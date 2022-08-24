#include "camera.hpp"
#include <thread>
#include <chrono>

class Camera_none : public Camera {
    public:
        Camera_none();
        ~Camera_none();
        virtual RawImage grab_frame();
};

extern "C" {
    std::unique_ptr<Camera> init_camera() {
       return std::make_unique<Camera_none>();
    }
}

Camera_none::Camera_none() : Camera() {
}

Camera_none::~Camera_none() {
}

RawImage Camera_none::grab_frame() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // emulate 10 fps
    const unsigned int width = 8;
    const unsigned int height = 8;
    const std::vector<unsigned char> data(width * height * 3);
    return RawImage(data, width, height, TJPF_RGB);
}
