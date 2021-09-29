#include "serialize.hpp"
#include <vector>
#ifndef WORLD_H
#define WORLD_H

template <typename idType>
class abstractWorld {
public:
	size_t w, h;
	std::vector<std::vector<idType>> memory;

	abstractWorld() = default;
	abstractWorld(size_t width, size_t height) {
		w = width; h = height;
		memory = std::vector<std::vector<idType>>(w, std::vector<idType>());
		for (auto column : memory) column.reserve(h);
	};
	abstractWorld(size_t width, size_t height, idType val) {
		w = width; h = height;
		memory = std::vector<std::vector<idType>>(w, std::vector<idType>(h, val));
	};

	std::vector<idType>& operator[] (size_t i) {
		return memory[i];
	}

	NVX_SERIALIZABLE(&w, &h, &memory);
};

#endif /* WORLD_H */