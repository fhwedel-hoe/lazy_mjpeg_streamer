#pragma once

#include <turbojpeg.h>

#include "types.hpp"

mimetyped_data compress(const std::vector<unsigned char> & buffer, const int width, const int height, const TJCS colorSpace, const TJPF pixelFormat);
