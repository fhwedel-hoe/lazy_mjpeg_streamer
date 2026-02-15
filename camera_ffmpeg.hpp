#pragma once

#include "camera.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class Camera_ffmpeg : public Camera {
    public:
        Camera_ffmpeg();
        ~Camera_ffmpeg();
        virtual RawImage grab_frame();
    private:
        const char * source = nullptr;
        AVFormatContext * format_ctx = nullptr;
        AVCodecParameters * codecpar = nullptr;
        unsigned int video_stream_index = -1;
        AVCodecContext * codec_ctx = nullptr;
        AVPacket * packet = nullptr;
        AVFrame * frame = nullptr;
};