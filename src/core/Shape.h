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
#include "VData.h"

#include <string>
#include <vector>

struct ShapeTransform {
	real_t m_x, m_y, m_angle;
	bool m_mirror, m_flip;
};

class Shape {

private:
	stringtag_t m_name;
	ShapeTransform m_transform;
	Cow<VData::Dict> m_params;
	bool m_selected;

public:
	inline Shape(stringtag_t name, const Cow<VData::Dict> &params)
		: m_name(name), m_params(params) {}
	inline Shape(stringtag_t name, Cow<VData::Dict> &&params)
		: m_name(name), m_params(std::move(params)) {}
	inline Shape(stringtag_t name, const VData::Dict &params)
		: m_name(name), m_params(std::make_shared<VData::Dict>(params)) {}
	inline Shape(stringtag_t name, VData::Dict &&params)
		: m_name(name), m_params(std::make_shared<VData::Dict>(std::move(params))) {}

	inline stringtag_t GetName() const { return m_name; }
	inline       ShapeTransform& GetTransform()       { return m_transform; }
	inline const ShapeTransform& GetTransform() const { return m_transform; }
	inline       Cow<VData::Dict>& GetParams()       { return m_params; }
	inline const Cow<VData::Dict>& GetParams() const { return m_params; }
	inline bool IsSelected() {return m_selected; }

};
