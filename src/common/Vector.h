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

struct Vertex {
	real_t x, y;
	inline Vertex() {}
	inline Vertex(double x, double y) : x(x), y(y) {}
};

struct GLvec2 {
	float x, y;
	inline GLvec2() {}
	inline GLvec2(float x, float y) : x(x), y(y) {}
	inline GLvec2(const Vertex& v) : x(v.x), y(v.y) {}
	inline operator float*() { return &x; }
};

struct GLvec3 {
	float x, y, z;
	inline GLvec3() {}
	inline GLvec3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline operator float*() { return &x; }
};

struct GLvec4 {
	float x, y, z, w;
	inline GLvec4() {}
	inline GLvec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline operator float*() { return &x; }
};

static_assert(sizeof(GLvec2) == sizeof(float) * 2, "Invalid GLvec2 struct!");
static_assert(sizeof(GLvec3) == sizeof(float) * 3, "Invalid GLvec3 struct!");
static_assert(sizeof(GLvec4) == sizeof(float) * 4, "Invalid GLvec4 struct!");
