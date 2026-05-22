#pragma once
#include <string>

namespace hash {
struct MurMurHash {
	size_t operator()(const std::string& key) const noexcept;
};
}