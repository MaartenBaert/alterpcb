#pragma once

#include "Basics.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <string>

namespace MurmurHash {

inline hash_t Rotateleft(hash_t x, uint32_t r) {
	return (x << r) | (x >> (32 - r));
}

inline hash_t HashData(hash_t hash, uint32_t data) {
	data *= 0xcc9e2d51;
	data = Rotateleft(data, 15);
	data *= 0x1b873593;
	hash ^= data;
	hash = Rotateleft(hash, 13);
	hash = hash * 5 + 0xe6546b64;
	return hash;
}

inline hash_t HashData(hash_t hash, uint64_t data) {
	hash = HashData(hash, (uint32_t) data);
	hash = HashData(hash, (uint32_t) (data >> 32));
	return hash;
}

inline hash_t HashData(hash_t hash, const void *data, size_t size) {
	size_t word_count = size / 4;
#ifdef MAY_ALIAS
	const uint32_t MAY_ALIAS *words = (const uint32_t*) data;
	for(size_t i = 0; i < word_count; ++i) {
		hash = HashData(hash, words[i]);
	}
#else
	const uint8_t *bytes = data;
	for(size_t i = 0; i < word_count; ++i) {
		const uint8_t *word = bytes + i * 4;
		uint32_t value = (uint32_t) word[0] | ((uint32_t) word[1] << 8) | ((uint32_t) word[2] << 16) | ((uint32_t) word[3] << 24);
		hash = HashData32(hash, value);
	}
#endif
	const uint8_t *remainder = (const uint8_t*) data + word_count * 4;
	uint32_t value = 0;
	switch(size & 3) {
		case 3: value |= (uint32_t) remainder[2] << 16;
		case 2: value |= (uint32_t) remainder[1] << 8;
		case 1: value |= (uint32_t) remainder[0];
		hash = HashData(hash, value);
	}
	return hash;
}

inline hash_t HashFinish(hash_t hash) {
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;
	return hash;
}

inline hash_t HashTruncate(hash_t hash, uint32_t bits) {
	return hash & ((1 << bits) - 1);
}

}
