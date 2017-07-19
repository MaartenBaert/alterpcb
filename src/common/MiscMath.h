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

#include "Basics.h"

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
	return fminf(fmaxf(v, lo), hi);
}
template<> inline double clamp<double>(double v, double lo, double hi) {
	assert(lo <= hi);
	return fmin(fmax(v, lo), hi);
}

template<typename F>
inline F ToRadians(F x) {
	return x * (F) (M_PI / 180.0);
}
template<typename F>
inline F ToDegrees(F x) {
	return x * (F) (180.0 / M_PI);
}

// IsPow2(0) = true
// IsPow2(1) = true
inline bool IsPow2(uint32_t x) {
	return ((x & (x - 1)) == 0);
}

// NextPow2(0) = 0
// NextPow2(1) = 1
inline uint32_t NextPow2(uint32_t x) {
	--x;
	x |= (x >>  1);
	x |= (x >>  2);
	x |= (x >>  4);
	x |= (x >>  8);
	x |= (x >> 16);
	return x + 1;
}

// FloorLog2(0) = 0
// FloorLog2(1) = 0
inline uint32_t FloorLog2(uint32_t x) {
	uint32_t i = 0;
	if(x >= ((uint32_t) 1 << 16)) { i += 16; x >>= 16; }
	if(x >= ((uint32_t) 1 <<  8)) { i +=  8; x >>=  8; }
	if(x >= ((uint32_t) 1 <<  4)) { i +=  4; x >>=  4; }
	if(x >= ((uint32_t) 1 <<  2)) { i +=  2; x >>=  2; }
	if(x >= ((uint32_t) 1 <<  1)) { i +=  1; }
	return i;
}
inline uint32_t FloorLog2(uint64_t x) {
	uint32_t i = 0;
	if(x >= ((uint64_t) 1 << 32)) { i += 32; x >>= 32; }
	if(x >= ((uint64_t) 1 << 16)) { i += 16; x >>= 16; }
	if(x >= ((uint64_t) 1 <<  8)) { i +=  8; x >>=  8; }
	if(x >= ((uint64_t) 1 <<  4)) { i +=  4; x >>=  4; }
	if(x >= ((uint64_t) 1 <<  2)) { i +=  2; x >>=  2; }
	if(x >= ((uint64_t) 1 <<  1)) { i +=  1; }
	return i;
}

// CeilLog2(0) = 0
// CeilLog2(1) = 0
inline uint32_t CeilLog2(uint32_t x) {
	return (x < 2)? 0 : FloorLog2(x - 1) + 1;
}
inline uint32_t CeilLog2(uint64_t x) {
	return (x < 2)? 0 : FloorLog2(x - 1) + 1;
}

template<typename T, typename U>
inline T MemCast(U value) {
	static_assert(sizeof(T) == sizeof(U), "Size of types does not match!");
	T res;
	memcpy(&res, &value, sizeof(T));
	return res;
}

// calculates (a * b) >> 64 without overflow
inline uint64_t FixMul64F(uint64_t a, uint64_t b) {
#if defined(__GNUC__) && defined(__SIZEOF_INT128__)
	typedef unsigned __int128 UINT128;
	static_assert(__SIZEOF_INT128__ == 16, "Size of 128-bit integer should be 16 bytes!");
	static_assert(sizeof(UINT128) == 16, "Size of 128-bit integer should be 16 bytes!");
	UINT128 c = (UINT128) a * (UINT128) b;
	return (uint64_t) (c >> 64);
#else
	uint32_t a0 = (uint32_t) a, a1 = (uint32_t) (a >> 32);
	uint32_t b0 = (uint32_t) b, b1 = (uint32_t) (b >> 32);
	uint64_t c00 = (uint64_t) a0 * (uint64_t) b0;
	uint64_t c01 = (uint64_t) a0 * (uint64_t) b1;
	uint64_t c10 = (uint64_t) a1 * (uint64_t) b0;
	uint64_t c11 = (uint64_t) a1 * (uint64_t) b1;
	uint64_t mid = (c00 >> 32) + (c01 & 0xffffffff) + (c10 & 0xffffffff);
	uint64_t hi = c11 + (c01 >> 32) + (c10 >> 32);
	return hi + (mid >> 32);
#endif
}

// calculates (a * b + (1 << 63)) >> 64 without overflow
inline uint64_t FixMul64R(uint64_t a, uint64_t b) {
#if defined(__GNUC__) && defined(__SIZEOF_INT128__)
	typedef unsigned __int128 UINT128;
	static_assert(__SIZEOF_INT128__ == 16, "Size of 128-bit integer should be 16 bytes!");
	static_assert(sizeof(UINT128) == 16, "Size of 128-bit integer should be 16 bytes!");
	UINT128 c = (UINT128) a * (UINT128) b;
	uint64_t lo = (uint64_t) c, hi = (uint64_t) (c >> 64);
	return hi + (lo >> 63);
#else
	uint32_t a0 = (uint32_t) a, a1 = (uint32_t) (a >> 32);
	uint32_t b0 = (uint32_t) b, b1 = (uint32_t) (b >> 32);
	uint64_t c00 = (uint64_t) a0 * (uint64_t) b0;
	uint64_t c01 = (uint64_t) a0 * (uint64_t) b1;
	uint64_t c10 = (uint64_t) a1 * (uint64_t) b0;
	uint64_t c11 = (uint64_t) a1 * (uint64_t) b1;
	uint64_t mid = (c00 >> 32) + (c01 & 0xffffffff) + (c10 & 0xffffffff);
	uint64_t hi = c11 + (c01 >> 32) + (c10 >> 32);
	return hi + ((mid + (UINT64_C(1) << 31)) >> 32);
#endif
}
