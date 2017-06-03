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

#include "ShapePrototype.h"

#include "ShapeDefinition.h"

void ShapePrototype::UpdateEffectiveParameters(ShapeDefinition *shape_definition) {
	m_effective_parameters.clear();
	if(shape_definition == NULL) {
		m_effective_parameters.reserve(m_parameters.GetSize());
		for(index_t i = 0; i < m_parameters.GetSize(); ++i) {
			m_effective_parameters.emplace_back(m_parameters[i].Key(), m_parameters[i].Value(), true, true);
		}
	} else {
		shape_definition->GetEffectiveParameters(m_effective_parameters, m_parameters);
	}
	m_effective_parameters_valid = true;
}

void ShapePrototype::UpdateTriangulation(ShapeDefinition *shape_definition) {
	m_triangulation.Clear();
	if(shape_definition != NULL)
		shape_definition->Triangulate(m_triangulation, GetEffectiveParameters(shape_definition));
	m_triangulation_valid = true;
}
