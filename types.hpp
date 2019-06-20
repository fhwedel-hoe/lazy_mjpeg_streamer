#pragma once

#include "publisher.hpp"

typedef std::vector<unsigned char> binary_data;

class IPC_globals {
    public:
        Publisher<unsigned int> readers;
        Publisher<binary_data> data;
        IPC_globals() : readers(0), data({}) {};
};