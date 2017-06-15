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
#include "Vector.h"

struct ShapeTransform {
	Vertex position;
	real_t angle;
	bool mirror, flip;
	real_t angle_sin, angle_cos;
	inline ShapeTransform() {}
	inline ShapeTransform(Vertex position, real_t angle, bool mirror, bool flip)
		: position(position), angle(angle), mirror(mirror), flip(flip) {
		UpdateAngleSinCos();
	}
	inline void UpdateAngleSinCos() {
		angle_sin = sin(ToRadians(angle));
		angle_cos = cos(ToRadians(angle));
	}
	inline Vertex TransformPoint(Vertex point) const {
		if(flip == mirror) {
			return Vertex(
				position.x + point.x * angle_cos - point.y * angle_sin,
				position.y + point.x * angle_sin + point.y * angle_cos
			);
		} else {
			return Vertex(
				position.x - point.x * angle_cos - point.y * angle_sin,
				position.y - point.x * angle_sin + point.y * angle_cos
			);
		}
	}
};

inline void CombineTransforms(ShapeTransform &dest, const ShapeTransform &a, const ShapeTransform &b) {
	dest.position = a.TransformPoint(b.position);
	dest.angle = remainder((a.flip == a.mirror)? a.angle + b.angle : a.angle - b.angle, 360.0);
	dest.mirror = (a.mirror != b.mirror);
	dest.flip = (a.flip != b.flip);
	dest.UpdateAngleSinCos();
}
