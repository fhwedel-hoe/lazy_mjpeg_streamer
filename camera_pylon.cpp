#include "camera_pylon.hpp"

#include <stdexcept>
#include <iostream>

extern "C" {
    std::unique_ptr<Camera> init_camera() {
       return std::make_unique<Camera_pylon>();
    }
}

Camera_pylon::Camera_pylon() : Camera() {
    // Before using any pylon methods, the pylon runtime must be initialized. 
    Pylon::PylonInitialize();
    
    // Create an instant camera object with the camera device found first.
    camera = new Pylon::CBaslerUsbInstantCamera( Pylon::CTlFactory::GetInstance().CreateFirstDevice());

    camera->Open();

    // Print the model name of the camera.
    std::cerr << "Using device " << camera->GetDeviceInfo().GetModelName() << std::endl;

    // The parameter MaxNumBuffer can be used to control the count of buffers
    // allocated for grabbing. The default value of this parameter is 10.
    camera->MaxNumBuffer = 5;

    camera->PixelFormat.SetValue(Basler_UsbCameraParams::PixelFormat_RGB8);
    //camera->Width.SetValue(640);
    //camera->Height.SetValue(480);
    //camera->OffsetX.SetValue(0);
    //camera->OffsetY.SetValue(0);
    camera->BinningHorizontal.SetValue(2);
    camera->BinningVertical.SetValue(2);

    // Start the grabbing.
    // The camera device is parameterized with a default configuration which
    // sets up free-running continuous acquisition.
    camera->StartGrabbing(/*Pylon::GrabStrategy_LatestImageOnly*/);

    // TODO: limit frame-rate
}

Camera_pylon::~Camera_pylon() {
    delete camera;
    Pylon::PylonTerminate(); 
}

RawImage Camera_pylon::grab_frame() {
        if ( camera->IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 1000 ms is used.
            camera->RetrieveResult(1000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
                // Access the image data.
                const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                const size_t payloadSize = ptrGrabResult->GetPayloadSize();
                const std::vector<unsigned char> vData(pImageBuffer,pImageBuffer+payloadSize);
                return RawImage(vData, ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight(), TJPF_RGB);
            }
            else
            {
                throw std::runtime_error(
                    std::string(ptrGrabResult->GetErrorDescription())
                );
            }
        }
        else
        {
            throw std::runtime_error("Camera is not grabbing.");
        }
}
