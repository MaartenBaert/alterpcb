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
#include "HashTable.h"
#include "VData.h"
#include "Vector.h"

#include <vector>

class EffectiveParameter {

private:
	stringtag_t m_name;
	VData m_value;
	bool m_override, m_orphan;

public:
	inline EffectiveParameter(stringtag_t name, const VData& value, bool override, bool orphan)
		: m_name(name), m_value(value), m_override(override), m_orphan(orphan) {}
	inline EffectiveParameter(stringtag_t name, VData&& value, bool override, bool orphan)
		: m_name(name), m_value(std::move(value)), m_override(override), m_orphan(orphan) {}

	// default copy and assignment
	EffectiveParameter(const EffectiveParameter&) = default;
	EffectiveParameter(EffectiveParameter&&) = default;
	EffectiveParameter& operator=(const EffectiveParameter&) = default;
	EffectiveParameter& operator=(EffectiveParameter&&) = default;

	inline stringtag_t GetName() const { return m_name; }
	inline const VData& GetValue() const { return m_value; }
	inline bool IsOverride() const { return m_override; }
	inline bool IsOrphan() const { return m_orphan; }

};

typedef std::vector<EffectiveParameter> EffectiveParameters;

struct TriangulationEntry {

private:
	stringtag_t m_layer;
	std::vector<GLvec2> m_triangles, m_edges;

public:
	inline TriangulationEntry(stringtag_t layer) : m_layer(layer) {}

	// default copy and assignment
	TriangulationEntry(const TriangulationEntry&) = default;
	TriangulationEntry(TriangulationEntry&&) = default;
	TriangulationEntry& operator=(const TriangulationEntry&) = default;
	TriangulationEntry& operator=(TriangulationEntry&&) = default;

	inline stringtag_t GetLayer() const { return m_layer; }
	inline const std::vector<GLvec2>& GetTriangles() const { return m_triangles; }
	inline const std::vector<GLvec2>& GetEdges() const { return m_edges; }

};

struct TriangulationHasher {
	inline bool Equal(const TriangulationEntry &a, const TriangulationEntry &b) const {
		return a.GetLayer() == b.GetLayer();
	}
	inline bool Equal(const TriangulationEntry &a, stringtag_t b) const {
		return a.GetLayer() == b;
	}
	inline hash_t Hash(hash_t hash, const TriangulationEntry &value) const {
		return MurmurHash::HashData(hash, value.GetLayer());
	}
	inline hash_t Hash(hash_t hash, stringtag_t value) const {
		return MurmurHash::HashData(hash, value);
	}
};

typedef HashTable<TriangulationEntry, TriangulationHasher> Triangulation;
