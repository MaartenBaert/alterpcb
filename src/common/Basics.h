/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>

typedef double real_t;
typedef uint32_t index_t;
typedef uint32_t stringtag_t;
typedef uint32_t hash_t;

constexpr index_t INDEX_NONE = (index_t) -1;
constexpr index_t STRINGTAG_NONE = (stringtag_t) -1;

#define UNUSED(x) ((void) (x))

#if defined(__GNUC__) && !defined(__clang__)
#define MAY_ALIAS __attribute((__may_alias__))
#endif

template<typename T>
inline T clamp(T v, T lo, T hi) {
	assert(lo <= hi);
	if(v < lo)
		return lo;
	if(v > hi)
		return hi;
	return v;
}
template<> inline float clamp<float>(float v, float lo, float hi) {
	assert(lo <= hi);
	return fmin(fmax(v, lo), hi);
}
template<> inline double clamp<double>(double v, double lo, double hi) {
	assert(lo <= hi);
	return fmin(fmax(v, lo), hi);
}

inline bool IsPow2(uint32_t x) {
	// IsPow2(0) = true
	// IsPow2(1) = true
	return ((x & (x - 1)) == 0);
}
inline uint32_t NextPow2(uint32_t x) {
	// NextPow2(0) = 0
	// NextPow2(1) = 1
	--x;
	x |= (x >>  1);
	x |= (x >>  2);
	x |= (x >>  4);
	x |= (x >>  8);
	x |= (x >> 16);
	return x + 1;
}
inline uint32_t FloorLog2(uint32_t x) {
	// FloorLog2(0) = 0
	// FloorLog2(1) = 0
	uint32_t i = 0;
	if(x >= ((uint32_t) 1 << 16)) { i += 16; x >>= 16; }
	if(x >= ((uint32_t) 1 <<  8)) { i +=  8; x >>=  8; }
	if(x >= ((uint32_t) 1 <<  4)) { i +=  4; x >>=  4; }
	if(x >= ((uint32_t) 1 <<  2)) { i +=  2; x >>=  2; }
	if(x >= ((uint32_t) 1 <<  1)) { i +=  1; }
	return i;
}
inline uint32_t CeilLog2(uint32_t x) {
	// CeilLog2(0) = 0
	// CeilLog2(1) = 0
	return (x < 2)? 0 : FloorLog2(x - 1) + 1;
}
