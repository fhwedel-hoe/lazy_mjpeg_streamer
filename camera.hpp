#pragma once

#include <turbojpeg.h>
#include <vector>
#include <memory>
#include <expected>

class RawImage {
    public:
        RawImage(const std::vector<unsigned char> data, const unsigned int width, const unsigned int height, const TJCS colorSpace, const TJPF pixelFormat) : 
        data(data), width(width), height(height), colorSpace(colorSpace), pixelFormat(pixelFormat) {};
        const std::vector<unsigned char> data;
        const unsigned int width;
        const unsigned int height;
        const TJCS colorSpace;
        const TJPF pixelFormat;
};

class Camera {
    public:
        class InitializationError : public std::runtime_error {
            using std::runtime_error::runtime_error;
        };
        class GrabError : public std::runtime_error {
            using std::runtime_error::runtime_error;
        };
        virtual std::expected<RawImage, Camera::GrabError> grab_frame() = 0;
        virtual ~Camera() {};
};

extern "C" {
    std::unique_ptr<Camera> init_camera();
}
