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
#include "LinkedList.h"
#include "ShapeData.h"
#include "VData.h"

class ShapeDefinition;

class ShapePrototype : public LinkedListElement<ShapePrototype> {

private:
	stringtag_t m_name;
	VData::Dict m_parameters;

	bool m_effective_parameters_valid;
	EffectiveParameters m_effective_parameters;

	bool m_triangulation_valid;
	Triangulation m_triangulation;

public:
	inline ShapePrototype(stringtag_t name, const VData::Dict &parameters)
		: m_name(name), m_parameters(parameters), m_triangulation_valid(false) {}
	inline ShapePrototype(stringtag_t name, VData::Dict &&parameters)
		: m_name(name), m_parameters(std::move(parameters)), m_triangulation_valid(false) {}

	// noncopyable
	ShapePrototype(const ShapePrototype&) = delete;
	ShapePrototype& operator=(const ShapePrototype&) = delete;

	inline stringtag_t GetName() const { return m_name; }
	inline const VData::Dict& GetParameters() const { return m_parameters; }

	inline const EffectiveParameters& GetEffectiveParameters(ShapeDefinition *shape_definition) const {
		if(!m_effective_parameters_valid)
			const_cast<ShapePrototype*>(this)->UpdateEffectiveParameters(shape_definition);
		return m_effective_parameters;
	}
	inline const Triangulation& GetTriangulation(ShapeDefinition *shape_definition) const {
		if(!m_triangulation_valid)
			const_cast<ShapePrototype*>(this)->UpdateTriangulation(shape_definition);
		return m_triangulation;
	}

private:
	void UpdateEffectiveParameters(ShapeDefinition *shape_definition);
	void UpdateTriangulation(ShapeDefinition *shape_definition);

};
