#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <thread>

#include "ueye.h"
#include "publisher.hpp"
#include "compress.hpp"
#include "serve.hpp"

void abortOnError(const int status, const char *msg) {
    if (status != 0) {
        throw std::runtime_error(std::string(msg)+std::to_string(status));
    }
}

void capture(IPC_globals & ipc) {
    
    for (;;) { // stream forever
    
        try { // do not break loop due to exceptions
      
            ipc.readers.read(); // wait for reader
            
            std::cerr << "Initializing camera for new recording session..." << std::endl;

            /* initialize first available camera */
            HIDS hCam = 0;
            int nRet = is_InitCamera(&hCam, 0);
            abortOnError(nRet, "InitCamera failed with error code: ");

            /* receive sensor info */
            SENSORINFO sensorInfo;
            nRet = is_GetSensorInfo(hCam, &sensorInfo);
            abortOnError(nRet, "Receving sensor info failed with error code: ");

            /* receive camera image size */
            IS_RECT rectAoi;
            nRet = is_AOI(hCam, IS_AOI_IMAGE_GET_AOI, &rectAoi, sizeof(rectAoi));
            abortOnError(nRet, "Receving image aoi failed with error code: ");

            /* set color mode */
            nRet = is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
            abortOnError(nRet, "Setting color mode failed with error code: ");

            /* allocate image buffer */
            char *pMemoryBuffer;
            int nMemoryId;
            nRet = is_AllocImageMem(hCam, rectAoi.s32Width, rectAoi.s32Height, 24, &pMemoryBuffer, &nMemoryId);
            abortOnError(nRet, "Allocating image memory failed with error code: ");
            size_t sData = rectAoi.s32Width * rectAoi.s32Height * 3;

            /* set allocated image buffer active */
            nRet = is_SetImageMem(hCam, pMemoryBuffer, nMemoryId);
            abortOnError(nRet, "Setting image memory active failed with error code: ");

            std::cerr << "Camera initialized, starting stream..." << std::endl;

            /* capture a single image and submit it to the streaming library */
            while (ipc.readers.read_unsafe() > 0) {
                
                /* capture a single frame */
                nRet = is_FreezeVideo(hCam, IS_WAIT);

                unsigned char* pData = reinterpret_cast<unsigned char*>(pMemoryBuffer);
                /* wrap C array */
                binary_data image_raw(pData,pData+sData);
                /* compress image data */
                binary_data image_compressed = 
                    compress(image_raw, rectAoi.s32Width, rectAoi.s32Height);

                /* publish for readers */
                ipc.data.publish(image_compressed);
            }
            
            std::cerr << "Stopping camera due to lack of viewers..." << std::endl;

            /* close camera */
            nRet = is_ExitCamera(hCam);
            abortOnError(nRet, "exit camera failed with error code: ");
        
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
        boost::asio::io_service io_service;
        server(io_service, server_port, ipc_globals);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
