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
#include "MiscMath.h"

#include <QtGui>

struct Color {
	float r, g, b, a;
	inline Color() {}
	inline Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
	inline Color(const QColor &color) : r(color.redF()), g(color.greenF()), b(color.blueF()), a(color.alphaF()) {}
	inline operator QColor() const { return QColor::fromRgbF(clamp(r, 0.0f, 1.0f), clamp(g, 0.0f, 1.0f), clamp(b, 0.0f, 1.0f), clamp(a, 0.0f, 1.0f)); }
};

inline Color Premultiply(const Color &color) {
	return Color(color.r * color.a, color.g * color.a, color.b * color.a, color.a);
}

inline float ToSRGB(float x) {
	return (x < 0.04045f / 12.92f)? 12.92f * x : 1.055f * pow(x, 1.0f / 2.4f) - 0.055f;
}

inline float FromSRGB(float x) {
	return (x < 0.04045f)? (1.0f / 12.92f) * x : pow((x + 0.055f) / 1.055f, 2.4f);
}

inline Color ToSRGB(const Color &color, bool dark = true) {
	return Color(ToSRGB(color.r), ToSRGB(color.g), ToSRGB(color.b), (dark)? ToSRGB(color.a) : 1.0f - ToSRGB(1.0f - color.a));
}

inline Color FromSRGB(const Color &color, bool dark = true) {
	return Color(FromSRGB(color.r), FromSRGB(color.g), FromSRGB(color.b), (dark)? FromSRGB(color.a) : 1.0f - FromSRGB(1.0f - color.a));
}

Color LumaInvert(const Color &color, bool invert = true);
