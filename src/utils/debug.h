#pragma once

#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>

namespace debug {

template<typename... Args>
std::string FormatString(const std::string& format, Args... args) {
    int str_size = std::snprintf(nullptr, 0, format.c_str(), args...);
    if (str_size <= 0) {
        throw std::runtime_error("Invalid string format");
    }

    size_t buf_size = static_cast<size_t>(str_size + 1);
    std::unique_ptr<char[]> buf(new char[buf_size]);
    std::snprintf(buf.get(), buf_size, format.c_str(), args...);
    
    return std::string(buf.get(), buf.get() + buf_size - 1);
}

} // namespace debug
