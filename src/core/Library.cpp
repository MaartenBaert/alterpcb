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
#include "LayerStack.h"

Library::Library(LibraryManager *parent,const std::string &name, const std::string &filename, LibraryType type) : LibraryTreeItem(LIBRARYTREEITEMTYPE_LIBRARY) {
	m_parent = parent;
	m_name = name;
	m_file_name = filename;
	m_type = type;
}

Library::~Library() {
	// nothing
}

Drawing *Library::NewDrawing(stringtag_t name, DrawingType type, stringtag_t layerstack) {
	TrackingPointer<Drawing> drawing(new Drawing(this,name, type, layerstack));
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

LayerStack *Library::NewLayerStack(stringtag_t name)
{
	TrackingPointer<LayerStack> layerstack(new LayerStack(name));
	LayerStack *ptr = layerstack.Get();
	m_layerstacks.emplace_back(std::move(layerstack));
	return ptr;
}

size_t Library::GetDrawingIndex(Drawing *drawing) {
	switch(drawing->GetType()) {
		case DRAWINGTYPE_SCHEMATIC: return IndexInVector(m_schematics, drawing);
		case DRAWINGTYPE_SYMBOL: return IndexInVector(m_symbols, drawing);
		case DRAWINGTYPE_LAYOUT: return IndexInVector(m_layouts, drawing);
	}
	assert(false);
	return 0;
}

LayerStack *Library::GetLayerStack(stringtag_t layerstack_name)
{
	for(index_t i = 0 ; i < m_layerstacks.size(); ++i) {
		if(m_layerstacks[i]->GetName() == layerstack_name){
			return m_layerstacks[i].Get();
		}
	}

	return (new LayerStack(STRINGTAG_NONE));

	// should not be reached
	assert(false);
	return 0;
}

