#pragma once

#include <turbojpeg.h>
#include <vector>

binary_data compress(binary_data buffer, int width, int height) {
    const int JPEG_QUALITY = 75;
    const int COLOR_COMPONENTS = 3;
    long unsigned int jpegSize = 0;
    unsigned char* compressedImage = 0;

    tjhandle _jpegCompressor = tjInitCompress();
    tjCompress2(
        _jpegCompressor, 
        buffer.data(), 
        width, 0, height, TJPF_RGB,
        &compressedImage, &jpegSize, 
        TJSAMP_444, JPEG_QUALITY, TJFLAG_FASTDCT
    );
    tjDestroy(_jpegCompressor);
    binary_data image_compressed(compressedImage,compressedImage+jpegSize);
    tjFree(compressedImage);
    return image_compressed;
}