#pragma once

#include "publisher.hpp"

class mimetyped_data {
    public:
    // TODO: can I const these fields?
    std::vector<unsigned char> data;
    std::string mimetype;
    mimetyped_data(std::vector<unsigned char> data, std::string mimetype) : data(data), mimetype(mimetype) {};
};

class IPC_globals {
    public:
        Publisher<unsigned int> readers;
        Publisher<mimetyped_data> data;
        IPC_globals() : readers(0), data(mimetyped_data({}, "application/x-empty")) {};
};