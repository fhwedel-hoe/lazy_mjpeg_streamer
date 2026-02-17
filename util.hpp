#pragma once
#include <optional>

namespace util {

template<typename T>
std::optional<T> getenv(const char * name);
}