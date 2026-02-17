#include "util.hpp"

#include <string>
#include <cstdlib>

template<>
std::optional<int> util::getenv<int>(const char * name) {
    const char *cstrval = std::getenv(name);
    if (nullptr != cstrval) {    
        return std::stoi(cstrval);
    }
    return std::nullopt;
}

template<>
std::optional<std::string> util::getenv<std::string>(const char * name) {
    const char *cstrval = std::getenv(name);
    if (nullptr != cstrval) {    
        return cstrval;
    }
    return std::nullopt;
}