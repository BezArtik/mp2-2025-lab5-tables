#include "hash/murmurhash.hpp"
#include <string>
#include <cstdint>

namespace hash {

size_t MurMurHash::operator()(const std::string& key) const noexcept {
    constexpr uint64_t m = 0xc6a4a7935bd1e995;
    constexpr int r = 47;
    uint64_t h = 0x8445d61a4e774912 ^ (key.size() * m);
    const uint64_t* data = reinterpret_cast<const uint64_t*>(key.data());
    const uint64_t* end = data + (key.size() / 8);
    while (data != end) {
        uint64_t k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }
    const uint8_t* data2 = reinterpret_cast<const uint8_t*>(data);
    switch (key.size() & 7) {
    case 7: h ^= static_cast<uint64_t>(data2[6]) << 48; break;
    case 6: h ^= static_cast<uint64_t>(data2[5]) << 40; break;
    case 5: h ^= static_cast<uint64_t>(data2[4]) << 32; break;
    case 4: h ^= static_cast<uint64_t>(data2[3]) << 24; break;
    case 3: h ^= static_cast<uint64_t>(data2[2]) << 16; break;
    case 2: h ^= static_cast<uint64_t>(data2[1]) << 8;  break;
    case 1: h ^= static_cast<uint64_t>(data2[0]); h *= m;
    }
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return static_cast<size_t>(h);
}


}