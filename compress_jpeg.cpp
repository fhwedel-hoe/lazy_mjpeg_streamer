#include "compress.hpp"
#include <vector>

binary_data compress(const binary_data & buffer, const int width, const int height, const TJPF pixelFormat) {
    const int JPEG_QUALITY = 85;
    long unsigned int jpegSize = 0;
    unsigned char* compressedImage = 0;

    tjhandle _jpegCompressor = tjInitCompress();
    tjCompress2(
        _jpegCompressor, 
        buffer.data(), 
        width, 0, height, pixelFormat,
        &compressedImage, &jpegSize, 
        TJSAMP_444, JPEG_QUALITY, TJFLAG_FASTDCT
    );
    tjDestroy(_jpegCompressor);
    binary_data image_compressed(compressedImage,compressedImage+jpegSize);
    tjFree(compressedImage);
    return image_compressed;
}
