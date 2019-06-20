#include "camera_realsense.hpp"

#include <stdexcept>
#include <iostream>

extern "C"
std::unique_ptr<Camera> init_camera() {
    return std::make_unique<Camera_realsense>();
}

Camera_realsense::Camera_realsense() : Camera(), ctx() {
    // Create a context object. This object owns the handles to all connected realsense devices.
    std::cerr << "There are " << ctx.get_device_count() << " connected RealSense devices." << std::endl;
    if(ctx.get_device_count() == 0) {
        throw std::runtime_error("No RealSense devices found.");
    }

    // This tutorial will access only a single device, but it is trivial to extend to multiple devices
    dev = ctx.get_device(0);
    std::cerr << "Using device 0, an "<< dev->get_name() << "\n"
    << "    Serial number: "<< dev->get_serial() << "\n"
    << "    Firmware version: "<< dev->get_firmware_version() << std::endl;

    // Configure depth to run at VGA resolution at 30 frames per second
    //dev->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 30);
    dev->enable_stream(rs::stream::color, width, height, rs::format::rgb8, 30);

    dev->start();

    // Determine depth value corresponding to one meter
    //const uint16_t one_meter = static_cast<uint16_t>(1.0f / dev->get_depth_scale());
}

Camera_realsense::~Camera_realsense() {
    dev->stop();
    dev->disable_stream(rs::stream::color);
}

RawImage Camera_realsense::grab_frame() {
    // This call waits until a new coherent set of frames is available on a device
    // Calls to get_frame_data(...) and get_frame_timestamp(...) on a device will return stable values until wait_for_frames(...) is called
    dev->wait_for_frames();

    // Retrieve depth data, which was previously configured as a 640 x 480 image of 16-bit depth values
    //const uint16_t * depth_frame = reinterpret_cast<const uint16_t *>(dev->get_frame_data(rs::stream::depth));

    const unsigned int sData = width * height * 3;
    const unsigned char * pData = reinterpret_cast<const unsigned char *>(dev->get_frame_data(rs::stream::color));
    const std::vector<unsigned char> vData(pData,pData+sData);
    return RawImage(vData, width, height);
}