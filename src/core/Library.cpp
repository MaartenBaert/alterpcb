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

#include "Library.h"

#include "Drawing.h"

Library::Library(LibraryManager *parent,const std::string &name, const std::string &filename, LibraryType type) : LibraryTreeItem(LIBRARYTREEITEMTYPE_LIBRARY) {
	m_parent = parent;
	m_name = name;
	m_file_name = filename;
	m_type = type;
}

Library::~Library() {
	// nothing
}

Drawing *Library::NewDrawing(stringtag_t name, DrawingType type) {
	ForwardPointer<Drawing> drawing(new Drawing(this,name, type));
	Drawing *ptr = drawing.Get();
	switch(type) {
		case DRAWINGTYPE_SCHEMATIC: m_schematics.emplace_back(std::move(drawing)); break;
		case DRAWINGTYPE_SYMBOL: m_symbols.emplace_back(std::move(drawing)); break;
		case DRAWINGTYPE_LAYOUT: m_layouts.emplace_back(std::move(drawing)); break;
	}
	return ptr;
}

void Library::DeleteDrawing(Drawing *drawing) {
	switch(drawing->GetType()) {
		case DRAWINGTYPE_SCHEMATIC: DeleteFromVector(m_schematics, drawing); break;
		case DRAWINGTYPE_SYMBOL: DeleteFromVector(m_symbols, drawing); break;
		case DRAWINGTYPE_LAYOUT: DeleteFromVector(m_layouts, drawing); break;
	}
}

ForwardPointer<Drawing> Library::MoveOutDrawing(Drawing *drawing)
{
	ForwardPointer<Drawing> ptr;
	switch(drawing->GetType()) {
		case DRAWINGTYPE_SCHEMATIC: ptr = MoveFromVector(m_schematics, drawing); break;
		case DRAWINGTYPE_SYMBOL: ptr = MoveFromVector(m_symbols, drawing); break;
		case DRAWINGTYPE_LAYOUT: ptr = MoveFromVector(m_layouts, drawing); break;
	}
	drawing->SetParent(nullptr);
	return ptr;
}

void Library::MoveInDrawing(ForwardPointer<Drawing> *ptr)
{
	std::cerr << "A" << std::endl;
	ptr->Get()->SetParent(this);
	switch(ptr->Get()->GetType()) {
		case DRAWINGTYPE_SCHEMATIC: m_schematics.push_back(std::move(*ptr)); break;
		case DRAWINGTYPE_SYMBOL: m_symbols.push_back(std::move(*ptr)); break;
		case DRAWINGTYPE_LAYOUT: m_layouts.push_back(std::move(*ptr)); break;
	}
}

void Library::MovePositionDrawing(Drawing *drawing, size_t target_index)
{
	moveInVector(m_layouts,IndexInVector(m_layouts,drawing),target_index);
}

LibraryManager *Library::GetParent() {
	return m_parent;
}


