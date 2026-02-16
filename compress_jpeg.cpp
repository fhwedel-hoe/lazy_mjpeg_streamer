#include "compress.hpp"
#include <vector>
#include <iostream>

binary_data compress(const binary_data & buffer, const int width, const int height, const TJCS colorSpace, const TJPF pixelFormat) {
    const int JPEG_QUALITY = 85;
    long unsigned int jpegSize = 0;
    unsigned char* compressedImage = 0;

    tjhandle _jpegCompressor = tjInitCompress();
    if (colorSpace == TJCS_RGB) {
        tjCompress2(
            _jpegCompressor, 
            buffer.data(), 
            width, 0, height, pixelFormat,
            &compressedImage, &jpegSize, 
            TJSAMP_420, JPEG_QUALITY, TJFLAG_FASTDCT
        );
    } else if (colorSpace == TJCS_YCbCr) {
        // TODO: fix ifdef – we should check the turbojpeg version here (Arch probably uses something more recent than Ubuntu 24)
        #ifdef ARCHLINUX
        tj3Set(_jpegCompressor, TJPARAM_SUBSAMP, TJSAMP_420);
        tj3Set(_jpegCompressor, TJPARAM_QUALITY, JPEG_QUALITY);
        tj3Set(_jpegCompressor, TJPARAM_FASTDCT, 1);
        if (-1 == tj3CompressFromYUV8
            (_jpegCompressor,
            buffer.data(), width, 1, height, 
            &compressedImage, &jpegSize
        )) {
            std::cerr << tjGetErrorStr() << std::endl;
        }
        #else
        if (-1 == tjCompressFromYUV(
            _jpegCompressor,
            buffer.data(), width, 1, height, TJSAMP_420, 
            &compressedImage, &jpegSize, 
            JPEG_QUALITY, TJFLAG_FASTDCT
        )) {
            std::cerr << tjGetErrorStr() << std::endl;
        }
        #endif
    }
    tjDestroy(_jpegCompressor);
    binary_data image_compressed(compressedImage,compressedImage+jpegSize);
    tjFree(compressedImage);
    return image_compressed;
}
