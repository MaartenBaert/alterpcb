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

#include "Drawing.h"

Drawing::Drawing(Library *parent, stringtag_t name, DrawingType type, stringtag_t layerstack) : LibraryTreeItem(LIBRARYTREEITEMTYPE_DRAWING){
	m_parent = parent;
	m_name = name;
	m_type = type;
	m_layerstack = layerstack;
}

void Drawing::HistoryClear() {
	m_history.clear();
	m_history.emplace_back(false);
}

void Drawing::HistoryRevert() {
	//TODO//
}

void Drawing::HistoryPush(bool soft) {
	UNUSED(soft);
	//TODO//
}

void Drawing::HistoryUndo() {
	//TODO//
}

void Drawing::HistoryRedo() {
	//TODO//
}

