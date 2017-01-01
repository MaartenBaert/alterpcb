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

#include "BackPointer.h"
#include "StringRegistry.h"
#include "CoreBasics.h"
#include "Cow.h"
#include "LibraryTreeItem.h"

#include <string>
#include <vector>

class Shape;
class Library;

class DrawingHistory {

private:
	bool m_soft;
	std::vector<Cow<Shape>> m_shapes;

public:
	inline DrawingHistory(bool soft) : m_soft(soft) {}

	// default copy and assignment
	DrawingHistory(const DrawingHistory&) = default;
	DrawingHistory(DrawingHistory&&) = default;
	DrawingHistory& operator=(const DrawingHistory&) = default;
	DrawingHistory& operator=(DrawingHistory&&) = default;

	inline bool IsSoft() const { return m_soft; }

	inline       std::vector<Cow<Shape>>& GetShapes()       { return m_shapes; }
	inline const std::vector<Cow<Shape>>& GetShapes() const { return m_shapes; }

};

class Drawing : public LibraryTreeItem, public BackPointer<Drawing> {

private:
	Library *m_parent;
	stringtag_t m_name;
	DrawingType m_type;
	std::vector<DrawingHistory> m_history;
	size_t m_history_position;

public:
	Drawing(Library *parent, stringtag_t name, DrawingType type);

	// noncopyable
	Drawing(const Drawing&) = delete;
	Drawing& operator=(const Drawing&) = delete;

	void HistoryClear();
	void HistoryRevert();
	void HistoryPush(bool soft = false);
	void HistoryUndo();
	void HistoryRedo();

	inline Library* GetParent() const {return m_parent;}
	inline stringtag_t GetName() const { return m_name; }
	inline DrawingType GetType() const { return m_type; }
	inline void SetParent(Library *parent) {m_parent = parent;}

	inline       std::vector<Cow<Shape>>& GetShapes()       { assert(!m_history.empty()); return m_history[m_history_position].GetShapes(); }
	inline const std::vector<Cow<Shape>>& GetShapes() const { assert(!m_history.empty()); return m_history[m_history_position].GetShapes(); }


};

struct DrawingCompare {
	inline bool operator()(const ForwardPointer<Drawing> &a, const ForwardPointer<Drawing> &b) const {
		return StringRegistry::GetString(a->GetName()) < StringRegistry::GetString(b->GetName());
	}
};
