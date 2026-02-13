#pragma once

#include "camera.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

class Camera_ffmpeg : public Camera {
    public:
        Camera_ffmpeg();
        ~Camera_ffmpeg();
        virtual RawImage grab_frame();
    private:
        const char * source;
        AVFormatContext * format_ctx;
};