#pragma once

#include <vector>
#include <memory>

class RawImage {
    public:
        RawImage(std::vector<unsigned char> data, unsigned int width, unsigned int height) : data(data), width(width), height(height) {};
        std::vector<unsigned char> data;
        unsigned int width;
        unsigned int height;
};

class Camera {
    public:
        virtual RawImage grab_frame() = 0;
};

extern "C"
std::unique_ptr<Camera> init_camera();