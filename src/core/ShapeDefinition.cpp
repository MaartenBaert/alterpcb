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

#include "ShapeDefinition.h"

#include "Json.h"
#include "StringRegistry.h"

ShapeDefinition::ShapeDefinition() {
	// nothing
}

ShapeDefinition::~ShapeDefinition() {
	// nothing
}

void ShapeDefinition::GetEffectiveParameters(EffectiveParameters &effective_parameters, const VData::Dict &parameters) {
	std::vector<bool> parameters_used(parameters.GetSize(), false);
	effective_parameters.reserve(m_parameters.size());
	for(index_t i = 0; i < m_parameters.size(); ++i) {
		ShapeDefinitionParameter &param = m_parameters[i];
		index_t index = parameters.Find(param.m_name);
		if(index == INDEX_NONE) {
			effective_parameters.emplace_back(param.m_name, param.m_default_value, false, false);
		} else {
			//TODO// verify & convert types
			effective_parameters.emplace_back(param.m_name, parameters[index].Value(), true, false);
			parameters_used[index] = true;
		}
	}
	for(index_t i = 0; i < parameters.GetSize(); ++i) {
		if(!parameters_used[i]) {
			effective_parameters.emplace_back(parameters[i].Key(), parameters[i].Value(), true, true);
		}
	}
	assert(effective_parameters.size() >= m_parameters.size());
}

ShapeDefinition_Rectangle::ShapeDefinition_Rectangle() {

	SHAPEDEFINITION_PARAM_INIT(layer  , "\"copper1-top\"");
	SHAPEDEFINITION_PARAM_INIT(x      , "0.0"            );
	SHAPEDEFINITION_PARAM_INIT(y      , "0.0"            );
	SHAPEDEFINITION_PARAM_INIT(width  , "2.0"            );
	SHAPEDEFINITION_PARAM_INIT(height , "2.0"            );
	SHAPEDEFINITION_PARAM_INIT(angle  , "0.0"            );
	SHAPEDEFINITION_PARAM_INIT(outline, "null"           );
	SHAPEDEFINITION_PARAM_INIT(hole   , "false"          );
	SHAPEDEFINITION_PARAM_INIT(order  , "0"              );
	SHAPEDEFINITION_PARAM_INIT(pad    , "false"          );

}

void ShapeDefinition_Rectangle::Triangulate(Triangulation &triangulation, const EffectiveParameters &effective_parameters) {
	UNUSED(triangulation);
	UNUSED(effective_parameters);

	//TODO// verify types
	/*std::string par_layer = effective_parameters[0].m_value.AsString(); //TODO// string to stringtag
	real_t x              = effective_parameters[1].m_value.AsFloat();
	real_t y              = effective_parameters[2].m_value.AsFloat();
	real_t width          = effective_parameters[3].m_value.AsFloat();
	real_t height         = effective_parameters[4].m_value.AsFloat();
	real_t angle          = effective_parameters[5].m_value.AsFloat();
	real_t outline        = effective_parameters[6].m_value.AsFloat(); //TODO// allow null
	bool hole             = effective_parameters[7].m_value.AsBool();
	int64_t order         = effective_parameters[8].m_value.AsInt();
	bool pad              = effective_parameters[9].m_value.AsBool();*/

	//TODO// implement this
	assert(false);

}
