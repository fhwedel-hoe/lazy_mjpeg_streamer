#include "camera_ffmpeg.hpp"
#include "util.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>

    std::unique_ptr<Camera> init_camera()
    {
        return std::make_unique<Camera_ffmpeg>();
    }
}

Camera_ffmpeg::Camera_ffmpeg() : Camera()
{
    std::optional<std::string> error;
    av_log_set_level(util::getenv<int>("FFMPEG_LOGLEVEL").value_or(16));
    avformat_network_init();
    source = std::getenv("FFMPEG_SOURCE");
    if (nullptr == source)
    {
        error = "FFMPEG_SOURCE environment variable not set.";
    }
    else
    {
        format_ctx = avformat_alloc_context();
        if (avformat_open_input(&format_ctx, source, nullptr, nullptr) < 0)
        {
            error = "Failed to open input: " + std::string(source);
        }
        else
        {

            if (avformat_find_stream_info(format_ctx, nullptr) < 0)
            {
                error = "Failed to find stream info";
            }
            else
            {
                //std::cout << "Format: " << format_ctx->iformat->name << std::endl;
                //std::cout << "Number of streams: " << format_ctx->nb_streams << std::endl;

                video_stream_index = -1;
                for (unsigned int i = 0; i < format_ctx->nb_streams; ++i)
                {
                    if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                    {
                        video_stream_index = i;
                        break;
                    }
                }
                if (video_stream_index == -1)
                {
                    error = "No video stream found.";
                }
                else
                {
                    codecpar = format_ctx->streams[video_stream_index]->codecpar;
                    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
                    if (!codec)
                    {
                        error = "Codec not found.";
                    }
                    else
                    {
                        codec_ctx = avcodec_alloc_context3(codec);
                        if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0)
                        {
                            error = "Failed to copy codec parameters.";
                        }
                        else if (avcodec_open2(codec_ctx, codec, nullptr) < 0)
                        {
                            error = "Failed to open codec.";
                        }
                        else
                        {
                            packet = av_packet_alloc();
                            frame = av_frame_alloc();
                            //std::cerr << "Codec ready to process frames." << std::endl;
                        }
                    }
                }
            }
        }
    }
    if (error.has_value()) {
        destroy();
        throw InitializationError(error.value());
    }
}

void Camera_ffmpeg::destroy() {
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_network_deinit();
}

Camera_ffmpeg::~Camera_ffmpeg() {
    destroy();
}

std::expected<RawImage, Camera::GrabError> Camera_ffmpeg::grab_frame()
{
    if (av_read_frame(format_ctx, packet) < 0) {
        return std::unexpected(GrabError("av_read_frame failed."));
    }
    // packet has been populated, remember to unref it no matter where we return
    std::unique_ptr<AVPacket, decltype(&av_packet_unref)> packet_guard(packet, &av_packet_unref);
    if (packet->stream_index != video_stream_index) {
        return std::unexpected(Camera::GrabError("Multi-stream input is not supported."));
    }
    if (avcodec_send_packet(codec_ctx, packet) != 0) {
        return std::unexpected(Camera::GrabError("avcodec_send_packet failed."));
    }
    if (avcodec_receive_frame(codec_ctx, frame) != 0) {
        return std::unexpected(Camera::GrabError("avcodec_receive_frame failed."));
    }
    //std::cout << "Decoded one frame: width=" << frame->width << ", height=" << frame->height << std::endl;
    if (frame->width != codecpar->width || frame->height != codecpar->height) {
        return std::unexpected(Camera::GrabError("Streams with variable frame dimensions are not supported."));
    }
    if (AV_PIX_FMT_YUV420P != static_cast<AVPixelFormat>(frame->format)) {
        return std::unexpected(Camera::GrabError(std::string("Only yuv420p is supported, but frame was ") + av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format))));
    }
    
    int num_bytes = av_image_get_buffer_size(static_cast<AVPixelFormat>(frame->format), frame->width, frame->height, 1);
    std::vector<unsigned char> buffer(num_bytes);
    av_image_copy_to_buffer(
        buffer.data(), num_bytes, frame->data, frame->linesize,
        static_cast<AVPixelFormat>(frame->format), frame->width, frame->height, 1
    );
    return RawImage(buffer, frame->width, frame->height, TJCS_YCbCr, TJPF_UNKNOWN);
}
