#pragma once

namespace ext {

namespace detail {

extern const unsigned crc32_table[256];

} // namespace detail


template<typename T>
unsigned crc32(unsigned crc, T begin, T end) {
    crc = ~crc;
    for (T iter = begin; iter != end; iter++) {
        crc = detail::crc32_table[(crc ^ *iter) & 0xff] ^ (crc >> 8);
    }
    return ~crc;
}

} // namespace ext
