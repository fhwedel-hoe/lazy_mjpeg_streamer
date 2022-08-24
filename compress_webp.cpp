#include "compress.hpp"
#include <webp/encode.h>
#include <type_traits>

binary_data compress(const binary_data & buffer, const int width, const int height, const TJPF pixelFormat) {
    const int & stride = width;
    const float quality_factor = 85;
    uint8_t *compressedImage = nullptr;
    size_t output_size = 0;
    if (pixelFormat == TJPF_RGB) {
        output_size = WebPEncodeRGB(buffer.data(), width, height, stride, quality_factor, &compressedImage);
    } else if (pixelFormat == TJPF_RGB) {
        output_size = WebPEncodeBGR(buffer.data(), width, height, stride, quality_factor, &compressedImage);
    } else {
        std::runtime_error("WebP needs RGB or BGR data. Input was neither.");
    }
    binary_data image_compressed(compressedImage, compressedImage+output_size);
    return image_compressed;
}
