#pragma once

#include "camera.hpp"
#include <librealsense/rs.hpp>

class Camera_realsense : public Camera {
    public:
        Camera_realsense();
        ~Camera_realsense();
        virtual RawImage grab_frame();
    private:
        rs::context ctx;
        rs::device * dev;
        const unsigned int width = 640;
        const unsigned int height = 480;
};