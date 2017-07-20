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

#include "TrackingPointer.h"
#include "StringRegistry.h"
#include "CoreBasics.h"
#include "Cow.h"
#include "LibraryTreeItem.h"

#include <string>
#include <vector>

class ShapeInstance;
class Library;

class DrawingHistory {

private:
	std::vector<Cow<ShapeInstance>> m_shapes;
	bool m_soft;

public:
	inline DrawingHistory() {
		assert(false); // should never be called, but std::vector::resize() needs a default constructor
	}
	inline DrawingHistory(bool soft)
		: m_soft(soft) {}
	inline DrawingHistory(std::vector<Cow<ShapeInstance>> &&shapes, bool soft)
		: m_shapes(std::move(shapes)), m_soft(soft) {}

	// default copy and assignment
	DrawingHistory(const DrawingHistory&) = default;
	DrawingHistory(DrawingHistory&&) = default;
	DrawingHistory& operator=(const DrawingHistory&) = default;
	DrawingHistory& operator=(DrawingHistory&&) = default;

	inline void Replace(std::vector<Cow<ShapeInstance>> &&shapes, bool soft) {
		m_shapes = std::move(shapes);
		m_soft = soft;
	}

	inline bool IsSoft() const { return m_soft; }

	inline const std::vector<Cow<ShapeInstance>>& GetShapes() const { return m_shapes; }

};

class Drawing : public LibraryTreeItem, public TrackingTarget<Drawing> {

private:
	Library *m_parent;
	stringtag_t m_name;
	DrawingType m_type;
	stringtag_t m_layerstack;

	std::vector<DrawingHistory> m_history;
	index_t m_history_position;

	static constexpr size_t HISTORY_SIZE = 20; // TODO: make this a user preference

public:
	Drawing(Library *parent, stringtag_t name, DrawingType type, stringtag_t layerstack);

	// noncopyable
	Drawing(const Drawing&) = delete;
	Drawing& operator=(const Drawing&) = delete;

	void HistoryClear(std::vector<Cow<ShapeInstance>> &&shapes);
	void HistoryPush(std::vector<Cow<ShapeInstance>> &&shapes, bool soft = false);
	void HistoryUndo();
	void HistoryRedo();
	inline bool Changed() {if(m_history_position > 0) {return true;} else {return false;}}; // poll to see if document has changed

	inline Library* GetParent() const { return m_parent; }
	inline void SetParentInternal(Library *parent) { m_parent = parent; }

	inline stringtag_t GetName() const { return m_name; }
	inline DrawingType GetType() const { return m_type; }
	inline stringtag_t GetLayerStack() const { return m_layerstack; }

	inline const std::vector<Cow<ShapeInstance>>& GetShapes() const {
		assert(m_history_position < m_history.size());
		return m_history[m_history_position].GetShapes();
	}

	inline void SetName(stringtag_t name) { m_name = name; }

};

/*struct DrawingCompare {
	inline bool operator()(const TrackingPointer<Drawing> &a, const TrackingPointer<Drawing> &b) const {
		return StringRegistry::GetString(a->GetName()) < StringRegistry::GetString(b->GetName());
	}
};*/
