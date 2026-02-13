#include "camera_ffmpeg.hpp"

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
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
    source = std::getenv("FFMPEG_SOURCE");
    if (nullptr == source)
    {
        std::cerr << "FFMPEG_SOURCE environment variable not set." << std::endl;
    }
    else
    {

        avformat_network_init();

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
                std::cout << "Duration: " << format_ctx->duration / AV_TIME_BASE << " seconds" << std::endl;
                std::cout << "Bit rate: " << format_ctx->bit_rate / 1000 << " kb/s" << std::endl;
                std::cout << "Number of streams: " << format_ctx->nb_streams << std::endl;
            }
        }
    }
}

Camera_ffmpeg::~Camera_ffmpeg()
{
    avformat_network_deinit();
}

RawImage Camera_ffmpeg::grab_frame()
{
    return RawImage(std::vector<unsigned char>(), 0, 0, TJPF_RGB);
}
