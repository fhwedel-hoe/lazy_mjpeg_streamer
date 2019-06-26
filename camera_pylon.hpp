#pragma once

#include "camera.hpp"
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

class Camera_pylon : public Camera {
    public:
        Camera_pylon();
        ~Camera_pylon();
        virtual RawImage grab_frame();
    private:
        // This smart pointer will receive the grab result data.
        Pylon::CGrabResultPtr ptrGrabResult;
        Pylon::CBaslerUsbInstantCamera * camera;
};