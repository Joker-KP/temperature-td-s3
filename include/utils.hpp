#pragma once

#include <Arduino.h>
#include <memory>

template<typename ... Args>
String stringFormat( const std::string& format, Args ... args )
{
    int size_s = snprintf(nullptr, 0, format.c_str(), args ...) + 1;  // Extra space for '\0'
    if (size_s <= 0) throw std::runtime_error("Error during formatting.");
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return String(buf.get());
}