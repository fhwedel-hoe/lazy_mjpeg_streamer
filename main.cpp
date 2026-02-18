#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <memory>
#include <dlfcn.h>

#include "publisher.hpp"
#include "compress.hpp"
#include "serve.hpp"
#include "camera.hpp"

constexpr static inline std::string_view errormessage_svg("<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n<svg version='1.1' xmlns='http://www.w3.org/2000/svg' xmlns:svg='http://www.w3.org/2000/svg'><text style='font-size:16px;font-family:monospace;' x='0' y='16' >ERROR: {}</text></svg>\n");

void capture(IPC_globals & ipc) {
    for (;;) { // stream forever
        try { // do not break loop due to exceptions
            if (ipc.readers.read_unsafe() == 0) {
                std::cerr << "Waiting for a reader…" << std::endl;
                ipc.readers.read();
            }
            std::cerr << "Initializing camera for new recording session..." << std::endl;
            std::unique_ptr<Camera> camera = init_camera();
            std::cerr << "Camera initialized, starting stream..." << std::endl;
            /* capture a single image and submit it to the streaming library */
            while (ipc.readers.read_unsafe() > 0) {
                /* grab raw image data frame */
                std::expected<RawImage, Camera::GrabError> expected_image = camera->grab_frame();
                if (expected_image.has_value()) {
                    RawImage & raw_image = expected_image.value();
                    /* compress image data */
                    mimetyped_data image_compressed = compress(raw_image.data, raw_image.width, raw_image.height, raw_image.colorSpace, raw_image.pixelFormat);
                    /* publish for readers */
                    ipc.data.publish(image_compressed);
                } else {
                    std::string message_image = std::format(errormessage_svg, expected_image.error().what());
                    ipc.data.publish(mimetyped_data(std::vector<unsigned char>(message_image.begin(), message_image.end()), "image/svg+xml"));
                }
            }
            std::cerr << "Stopping camera due to lack of viewers..." << std::endl; 
            // ^^ happens implicitly during destructor
        } catch (Camera::InitializationError & cie) {
            std::string message_image = std::format(errormessage_svg, cie.what());
            ipc.data.publish(mimetyped_data(std::vector<unsigned char>(message_image.begin(), message_image.end()), "image/svg+xml"));
            sleep(1);
        } catch (std::exception & se) {
            std::cerr << "Unexpected exception: " << se.what() << "\n";
        }
    }
}

IPC_globals ipc_globals;

const unsigned short server_port = 8080;

int main(int, char**) {
    std::thread(capture, std::ref(ipc_globals)).detach();
    try {
        boost::asio::io_context io_context;
        server(io_context, server_port, ipc_globals);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
