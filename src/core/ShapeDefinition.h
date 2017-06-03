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
#include "CoreBasics.h"
#include "LinkedList.h"
#include "ShapeData.h"
#include "VData.h"

struct ShapeDefinitionParameter {
	stringtag_t m_name;
	VData m_default_value;
	inline ShapeDefinitionParameter(stringtag_t name, const VData& default_value)
		: m_name(name), m_default_value(default_value) {}
	inline ShapeDefinitionParameter(stringtag_t name, VData&& default_value)
		: m_name(name), m_default_value(std::move(default_value)) {}
};

class ShapeDefinition : public LinkedListElement<ShapeDefinition> {

private:
	std::vector<ShapeDefinitionParameter> m_parameters;

public:
	ShapeDefinition();
	~ShapeDefinition();

	void GetEffectiveParameters(EffectiveParameters &effective_parameters, const VData::Dict &parameters);

	virtual void Triangulate(Triangulation &triangulation, const EffectiveParameters &effective_parameters) = 0;

protected:
	inline void AddParameter(stringtag_t name, const VData& default_value) {
		m_parameters.emplace_back(name, default_value);
	}
	inline void AddParameter(stringtag_t name, VData&& default_value) {
		m_parameters.emplace_back(name, std::move(default_value));
	}

public:
	inline const VData::Dict& GetDefaultParameters();

};

#define SHAPEDEFINITION_PARAM_DEFINE(name) \
	stringtag_t m_tag_##name;
#define SHAPEDEFINITION_PARAM_INIT(name, default_value) { \
	m_tag_##name = SRNewTag(#name); \
	AddParameter(m_tag_##name, Json::FromString(default_value)); \
}

class ShapeDefinition_Rectangle : public ShapeDefinition {

private:
	SHAPEDEFINITION_PARAM_DEFINE(layer)
	SHAPEDEFINITION_PARAM_DEFINE(x)
	SHAPEDEFINITION_PARAM_DEFINE(y)
	SHAPEDEFINITION_PARAM_DEFINE(width)
	SHAPEDEFINITION_PARAM_DEFINE(height)
	SHAPEDEFINITION_PARAM_DEFINE(angle)
	SHAPEDEFINITION_PARAM_DEFINE(outline)
	SHAPEDEFINITION_PARAM_DEFINE(hole)
	SHAPEDEFINITION_PARAM_DEFINE(order)
	SHAPEDEFINITION_PARAM_DEFINE(pad)

public:
	ShapeDefinition_Rectangle();

	virtual void Triangulate(Triangulation &triangulation, const EffectiveParameters &effective_parameters) override;

};
