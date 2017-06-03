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
#include <cstddef>
#include <cstdint>
#include <cstring>

typedef double real_t;
typedef uint32_t index_t;
typedef uint32_t stringtag_t;
typedef uint32_t hash_t;

constexpr index_t INDEX_NONE = (index_t) -1;
constexpr stringtag_t STRINGTAG_NONE = (stringtag_t) -1;

#define UNUSED(x) ((void) (x))

#if defined(__GNUC__) && !defined(__clang__)
#define MAY_ALIAS __attribute((__may_alias__))
#endif
