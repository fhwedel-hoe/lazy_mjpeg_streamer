#pragma once

#include <turbojpeg.h>

#include "types.hpp"

binary_data compress(const binary_data & buffer, const int width, const int height, const TJPF pixelFormat);
