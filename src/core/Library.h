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
#include "Basics.h"
#include "CoreBasics.h"
#include "LibraryTreeItem.h"

#include <string>
#include <vector>

class LibraryManager;
class Drawing;

class Library : public LibraryTreeItem, public TrackingTarget<Library> {

private:
	LibraryManager *m_parent;
	std::string m_name, m_file_name;
	LibraryType m_type;
	std::vector<TrackingPointer<Drawing>> m_schematics;
	std::vector<TrackingPointer<Drawing>> m_symbols;
	std::vector<TrackingPointer<Drawing>> m_layouts;

public:
	Library(LibraryManager *parent, const std::string &name, const std::string &filename, LibraryType type);
	~Library();

	// noncopyable
	Library(const Library&) = delete;
	Library& operator=(const Library&) = delete;

	Drawing* NewDrawing(stringtag_t name, DrawingType type);
	void DeleteDrawing(Drawing *drawing);

	size_t GetDrawingIndex(Drawing *drawing);

	inline LibraryManager* GetParent() { return m_parent; }
	inline const std::string& GetName() const { return m_name; }
	inline const std::string& GetFileName() const { return m_file_name; }
	inline LibraryType GetType() const { return m_type; }

	inline Drawing* GetSchematic(size_t index) { assert(index < m_schematics.size()); return m_schematics[index].Get(); }
	inline size_t GetSchematicCount() { return m_schematics.size(); }
	inline Drawing* GetSymbol(size_t index) { assert(index < m_symbols.size()); return m_symbols[index].Get(); }
	inline size_t GetSymbolCount() { return m_symbols.size(); }
	inline Drawing* GetLayout(size_t index) { assert(index < m_layouts.size()); return m_layouts[index].Get(); }
	inline size_t GetLayoutCount() { return m_layouts.size(); }

	inline void SetName(const std::string &name) {m_name = name;}
	inline void SetFilePath(const std::string &filename) {m_file_name = filename;}
};

struct LibraryCompare {
	inline bool operator()(const TrackingPointer<Library> &a, const TrackingPointer<Library> &b) const {
		return a->GetName() < b->GetName();
	}
};
