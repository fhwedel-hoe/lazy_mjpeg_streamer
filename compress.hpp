#pragma once

#include <turbojpeg.h>

#include "types.hpp"

binary_data compress(binary_data buffer, int width, int height, TJPF pixelFormat);
