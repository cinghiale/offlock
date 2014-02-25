#pragma once

#include <sstream>
#include <string>
#include <stdexcept>

namespace {
    using std::runtime_error;
    using std::ostringstream;

    void _supplant(ostringstream& buffer, char const* msg) {
        while(*msg) {
            if(*msg == '%') {
                throw runtime_error("invalid message: too many placeholders");
            }
            buffer << *msg++;
        }
    }

    template<typename T, typename... Args>
    void _supplant(ostringstream& buffer, char const* msg, T arg0, Args... args) {
        while(*msg) {
            if(*msg == '%') {
                buffer << arg0;
                return _supplant(buffer, msg+1, args...);
            }
            buffer << *msg++;
        }
        throw runtime_error("invalid message: too many parameters");
    }
};

template<typename... Args>
std::string supplant(char const* msg, Args... args) {
    std::ostringstream buffer;
    _supplant(buffer, msg, args...);
    return buffer.str();
}

