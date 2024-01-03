#pragma once

#include <ostream>
#include <string>
namespace IO {
template <class T, class CharT = std::char_traits<T>>
std::basic_ostream<T, CharT> &endl(std::basic_ostream<T, CharT> &outputStream) {
    return outputStream << outputStream.widen("\n");
}
} // namespace IO
