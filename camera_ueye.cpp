#include "camera_ueye.hpp"

#include <stdexcept>

extern "C"
std::unique_ptr<Camera> init_camera() {
    return std::make_unique<Camera_ueye>();
}

void abortOnError(const int status, const char *msg) {
    if (status != 0) {
        throw std::runtime_error(std::string(msg)+std::to_string(status));
    }
}

Camera_ueye::Camera_ueye() : Camera() {
    /* initialize first available camera */
    int nRet = is_InitCamera(&hCam, 0);
    abortOnError(nRet, "InitCamera failed with error code: ");

    /* receive sensor info */
    SENSORINFO sensorInfo;
    nRet = is_GetSensorInfo(hCam, &sensorInfo);
    abortOnError(nRet, "Receving sensor info failed with error code: ");

    /* receive camera image size */
    nRet = is_AOI(hCam, IS_AOI_IMAGE_GET_AOI, &rectAoi, sizeof(rectAoi));
    abortOnError(nRet, "Receving image aoi failed with error code: ");

    /* set color mode */
    nRet = is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
    abortOnError(nRet, "Setting color mode failed with error code: ");

    /* allocate image buffer */
    int nMemoryId;
    nRet = is_AllocImageMem(hCam, rectAoi.s32Width, rectAoi.s32Height, 24, &pMemoryBuffer, &nMemoryId);
    abortOnError(nRet, "Allocating image memory failed with error code: ");
    sData = rectAoi.s32Width * rectAoi.s32Height * 3;

    /* set allocated image buffer active */
    nRet = is_SetImageMem(hCam, pMemoryBuffer, nMemoryId);
    abortOnError(nRet, "Setting image memory active failed with error code: ");
}

Camera_ueye::~Camera_ueye() {
    /* close camera */
    int nRet = is_ExitCamera(hCam);
    abortOnError(nRet, "exit camera failed with error code: ");
}

RawImage Camera_ueye::grab_frame() {
    /* capture a single frame */
    int nRet = is_FreezeVideo(hCam, IS_WAIT);
    unsigned char* pData = reinterpret_cast<unsigned char*>(pMemoryBuffer);
    /* wrap C array */
    return RawImage(std::vector<unsigned char>(pData,pData+sData), rectAoi.s32Width, rectAoi.s32Height);
}