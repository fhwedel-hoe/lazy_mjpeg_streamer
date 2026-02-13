#include "camera_ffmpeg.hpp"

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}
#include <stdexcept>
#include <iostream>

extern "C"
{
    std::unique_ptr<Camera> init_camera()
    {
        return std::make_unique<Camera_ffmpeg>();
    }
}

Camera_ffmpeg::Camera_ffmpeg() : Camera()
{
    avformat_network_init();
    source = std::getenv("FFMPEG_SOURCE");
    if (nullptr == source)
    {
        std::cerr << "FFMPEG_SOURCE environment variable not set." << std::endl;
    }
    else
    {
        format_ctx = avformat_alloc_context();
        if (avformat_open_input(&format_ctx, source, nullptr, nullptr) < 0)
        {
            std::cerr << "Failed to open input: " << source << std::endl;
        }
        else
        {

            if (avformat_find_stream_info(format_ctx, nullptr) < 0)
            {
                std::cerr << "Failed to find stream info" << std::endl;
            }
            else
            {
                std::cout << "Format: " << format_ctx->iformat->name << std::endl;
                std::cout << "Number of streams: " << format_ctx->nb_streams << std::endl;

                video_stream_index = -1;
                for (unsigned int i = 0; i < format_ctx->nb_streams; ++i) {
                    if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                        video_stream_index = i;
                        break;
                    }
                }
                if (video_stream_index == -1) {
                    std::cerr << "No video stream found." << std::endl;
                } else {
                    AVCodecParameters* codecpar = format_ctx->streams[video_stream_index]->codecpar;
                    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
                    if (!codec) {
                        std::cerr << "Codec not found." << std::endl;
                    } else {
                        codec_ctx = avcodec_alloc_context3(codec);
                        if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
                            std::cerr << "Failed to copy codec parameters." << std::endl;
                        } else if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
                            std::cerr << "Failed to open codec." << std::endl;
                        } else {
                            packet = av_packet_alloc();
                            frame = av_frame_alloc();
                            std::cerr << "Codec ready to process frames." << std::endl;
                        }
                    }
                }
            }
        }
    }
}

Camera_ffmpeg::~Camera_ffmpeg()
{
    if (frame != nullptr)
    av_frame_free(&frame);
    if (packet != nullptr)
    av_packet_free(&packet);
    avformat_network_deinit();
}

RawImage Camera_ffmpeg::grab_frame()
{
    
    if (av_read_frame(format_ctx, packet) < 0) {
        std::cerr << "av_read_frame failed." << std::endl;
    } else {
        if (packet->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, packet) == 0) {
                if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    std::cout << "Decoded one frame: width=" << frame->width
                                << ", height=" << frame->height << std::endl;
                    AVFrame* rgb_frame = av_frame_alloc();
                    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
                    std::vector<unsigned char> buffer(num_bytes);

                    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer.data(), AV_PIX_FMT_RGB24, frame->width, frame->height, 1);

                    struct SwsContext* sws_ctx = sws_getContext(
                        frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
                        frame->width, frame->height, AV_PIX_FMT_RGB24,
                        SWS_BILINEAR, nullptr, nullptr, nullptr);

                    sws_scale(
                        sws_ctx,
                        frame->data, frame->linesize,
                        0, frame->height,
                        rgb_frame->data, rgb_frame->linesize);

                    RawImage output(buffer, frame->width, frame->height, TJPF_RGB);

                    sws_freeContext(sws_ctx);
                    av_frame_free(&rgb_frame);
                }
            }
        }
        av_packet_unref(packet);
    }
    return;
}
