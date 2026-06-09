#pragma once

#include "camera.hpp"
#include "ueye.h"

class Camera_ueye : public Camera {
    public:
        Camera_ueye();
        ~Camera_ueye();
        virtual std::expected<RawImage, Camera::GrabError> grab_frame();
    private:
        char *pMemoryBuffer = 0;
        size_t sData = 0;
        HIDS hCam = 0;
        IS_RECT rectAoi = {0};
};
