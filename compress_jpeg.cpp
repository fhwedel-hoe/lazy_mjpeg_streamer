#include "compress.hpp"
#include "util.hpp"
#include <vector>
#include <iostream>

binary_data compress(const binary_data & buffer, const int width, const int height, const TJCS colorSpace, const TJPF pixelFormat) {
    const int JPEG_QUALITY = util::getenv<int>("JPEG_QUALITY").value_or(85);
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
        if (-1 == tjCompressFromYUV(
            _jpegCompressor,
            buffer.data(), width, 1, height, TJSAMP_420, 
            &compressedImage, &jpegSize, 
            JPEG_QUALITY, TJFLAG_FASTDCT
        )) {
            std::cerr << tjGetErrorStr() << std::endl;
        }
    }
    tjDestroy(_jpegCompressor);
    binary_data image_compressed(compressedImage,compressedImage+jpegSize);
    tjFree(compressedImage);
    return image_compressed;
}
