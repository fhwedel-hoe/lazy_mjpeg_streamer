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
        virtual std::expected<RawImage, Camera::GrabError> grab_frame();
    private:
        void destroy();
        std::unique_ptr<AVFormatContext, void (*)(AVFormatContext *)> format_ctx = {nullptr, [](AVFormatContext *) {}};
        AVCodecParameters * codecpar = nullptr; // this will be used to provide easy acces to a struct managed by format_ctx, so no explicit deleter on this one
        unsigned int video_stream_index = -1;
        std::unique_ptr<AVCodecContext, void (*)(AVCodecContext *)> codec_ctx = {nullptr, [](AVCodecContext *) {}};
        std::unique_ptr<AVPacket, void (*)(AVPacket *)> packet = {nullptr, [](AVPacket *) {}};
        std::unique_ptr<AVFrame, void (*)(AVFrame *)> frame = {nullptr, [](AVFrame *) {}};
};