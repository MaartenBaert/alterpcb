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

#include "LibraryManager.h"

#include "Library.h"
#include "LayerStack.h"
#include "Drawing.h"
#include "ShapeDefinition.h"
#include "ShapeInstance.h"
#include "ShapePrototype.h"
#include "StringRegistry.h"
#include "Icons.h"

#include <iostream>

LibraryManager::LibraryManager() {
	// nothing
}

LibraryManager::~LibraryManager() {
	// nothing
}

Library *LibraryManager::NewLibrary(const std::string &name, const std::string &filename, LibraryType type) {
	TrackingPointer<Library> library(new Library(this, name, filename, type));
	Library *ptr = library.Get();
	m_libraries.emplace_back(std::move(library));
	m_empty_layerstack = new LayerStack(STRINGTAG_NONE);
	return ptr;
}

void LibraryManager::DeleteLibrary(Library *library) {
	DeleteFromVector(m_libraries, library);
	delete m_empty_layerstack;
}

void LibraryManager::AddShapeDefinition(stringtag_t name, ShapeDefinition *shape_definition) {
	index_t index = m_shape_trackers.TryEmplaceBack(name, name).first;
	shape_definition->InsertBack(&m_shape_trackers[index].m_shape_definitions);
}

void LibraryManager::AddShapeInstance(stringtag_t name, ShapeInstance *shape_instance) {
	index_t index = m_shape_trackers.TryEmplaceBack(name, name).first;
	shape_instance->InsertBack(&m_shape_trackers[index].m_shape_instances);
}

void LibraryManager::AddShapePrototype(stringtag_t name, ShapePrototype *shape_prototype) {
	index_t index = m_shape_trackers.TryEmplaceBack(name, name).first;
	shape_prototype->InsertBack(&m_shape_trackers[index].m_shape_prototypes);
}

QModelIndex LibraryManager::index(int row, int column, const QModelIndex &parent) const {

	// Check whether the requested index (i.e. row and column) is valid. We can already check the things that don't
	// depend on the type of item we are working with. The final row count check will be done later.
	if(row < 0)
		return QModelIndex();

	// is parent the root?
	if(!parent.isValid()) {
		if((size_t) row >= GetLibraryCount())
			return QModelIndex();
		return createIndex(row, column, static_cast<LibraryTreeItem*>(GetLibrary(row)));
	}

	// what type of parent is this?
	LibraryTreeItem *parent_ptr = (LibraryTreeItem*) parent.internalPointer();
	switch(parent_ptr->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY: {
			Library *library = static_cast<Library*>(parent_ptr);
			if((size_t) row >= library->GetLayoutCount())
				return QModelIndex();
			return createIndex(row, column, static_cast<LibraryTreeItem*>(library->GetLayout(row)));
		}
		case LIBRARYTREEITEMTYPE_DRAWING: {
			return QModelIndex(); // drawings have no childs
		}
	}

	// this should never be reached
	assert(false);
	return QModelIndex();

}

QModelIndex LibraryManager::parent(const QModelIndex &index) const {

	// the root has no parent
	if(!index.isValid())
		return QModelIndex();

	// what type of item is this?
	LibraryTreeItem *item_ptr = (LibraryTreeItem*) index.internalPointer();
	switch(item_ptr->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY: { // parent is root
			return QModelIndex();
		}
		case LIBRARYTREEITEMTYPE_DRAWING: { // parent is a library
			Drawing *drawing = static_cast<Drawing*>(item_ptr);
			Library *library = drawing->GetParent();
			return createIndex((int) IndexInVector(m_libraries, library), 0, static_cast<LibraryTreeItem*>(library));
		}
	}

	// this should never be reached
	assert(false);
	return QModelIndex();

}

int LibraryManager::rowCount(const QModelIndex &parent) const {

	// is parent the root?
	if(!parent.isValid())
		return (int) GetLibraryCount();

	// what type of parent is this?
	LibraryTreeItem *parent_ptr = (LibraryTreeItem*) parent.internalPointer();
	switch(parent_ptr->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY: {
			Library *library = static_cast<Library*>(parent_ptr);
			return (int) library->GetLayoutCount();
		}
		case LIBRARYTREEITEMTYPE_DRAWING: {
			return 0;
		}
	}

	// this should never be reached
	assert(false);
	return 0;

}

int LibraryManager::columnCount(const QModelIndex &parent) const {
	UNUSED(parent);
	return 2;
}

Qt::ItemFlags LibraryManager::flags(const QModelIndex &index) const {
	if(!index.isValid())
		return Qt::ItemIsDropEnabled; // dropping on root is allowed
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant LibraryManager::data(const QModelIndex &index, int role) const {

	if(!index.isValid())
		return QVariant();

	LibraryTreeItem *item_ptr = (LibraryTreeItem*) index.internalPointer();
	switch(role){
		case Qt::DisplayRole:
		case Qt::EditRole: {
			switch(item_ptr->GetTreeItemType()) {
				case LIBRARYTREEITEMTYPE_LIBRARY: {
					Library *library = static_cast<Library*>(item_ptr);
					if(index.column() == 0) {
						return QString::fromStdString(library->GetName());
					}
					else if (index.column() == 1) {
						return QString::fromStdString(library->GetFileName());
					}
					else {
						return QString("");
					}
				}
				case LIBRARYTREEITEMTYPE_DRAWING: {
					Drawing *drawing = static_cast<Drawing*>(item_ptr);
					if(index.column() == 0) {
						return QString::fromStdString(StringRegistry::GetString(drawing->GetName()));
					}
					else {
						return QString("");
					}
				}}}
		case Qt::DecorationRole: {
			switch(item_ptr->GetTreeItemType()) {
				case LIBRARYTREEITEMTYPE_LIBRARY: {
					return g_icon_librarymanager_library;
				}
				case LIBRARYTREEITEMTYPE_DRAWING: {
					Drawing *drawing = static_cast<Drawing*>(item_ptr);
					switch (drawing->GetType()) {
						case DRAWINGTYPE_SCHEMATIC:
							return g_icon_librarymanager_schematic;
						case DRAWINGTYPE_SYMBOL:
							return g_icon_librarymanager_symbol;
						case DRAWINGTYPE_LAYOUT:
							return g_icon_librarymanager_layout;
					}
					return QVariant();
				}}}
		case Qt::ForegroundRole: {
			if (index.column() == 1) { // color of the text to indicate if filepath is valid or not
				return QColor(Qt::blue);
			}
		}
		default: {
			return QVariant();
		}
	}
	// this should never be reached
	assert(false);
	return QVariant();

}

QVariant LibraryManager::headerData(int section, Qt::Orientation orientation,int role) const {
	UNUSED(section);
	UNUSED(orientation);
	UNUSED(role);
	return QVariant();
}

bool LibraryManager::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(role ==  Qt::EditRole) {
		if (!value.toString().isEmpty()) {
			layoutAboutToBeChanged();
			LibraryTreeItem *item_ptr = (LibraryTreeItem*) index.internalPointer();
			if (index.column() == 0) {
				switch(item_ptr->GetTreeItemType()) {
					case LIBRARYTREEITEMTYPE_LIBRARY: {
						Library *library = static_cast<Library*>(item_ptr);
						library->SetName((value.toString()).toStdString());
						layoutChanged();
						return true;
					}
					case LIBRARYTREEITEMTYPE_DRAWING: {
						Drawing *drawing = static_cast<Drawing*>(item_ptr);
						drawing->SetName(StringRegistry::NewTag((value.toString()).toStdString()));
						layoutChanged();
						return true;
					}}
			}
			else if (index.column() == 1) {
				Library *library = static_cast<Library*>(item_ptr);
				library->SetFilePath((value.toString()).toStdString());
				layoutChanged();
				return true;
			}
		}
	}

	return false;
}

Qt::DropActions LibraryManager::supportedDropActions() const {
	return Qt::MoveAction;
}

QStringList LibraryManager::mimeTypes() const {
	QStringList types;
	types.push_back("application/x-alterpcb-librarytreeitem");
	return types;
}

QMimeData* LibraryManager::mimeData(const QModelIndexList &indexes) const {
	std::unique_ptr<LibraryTreeItemsMime> mime_data(new LibraryTreeItemsMime());
	std::vector<SafePointer<LibraryTreeItem>> &items = mime_data->GetItems();
	for(int i = 0; i < indexes.size(); ++i) {
		const QModelIndex &index = indexes[i];
		if(!index.isValid())
			continue;
		items.emplace_back((LibraryTreeItem*) index.internalPointer());
	}
	mime_data->setData("application/x-alterpcb-librarytreeitem", QByteArray());
	return mime_data.release();
}

bool LibraryManager::dropLocation(const QMimeData *data, Qt::DropAction action, DropLocation location, int &row, int &column, QModelIndex &parent) {

	if(action == Qt::IgnoreAction)
		return true;

	if(!data->hasFormat("application/x-alterpcb-librarytreeitem"))
		return false;
	const LibraryTreeItemsMime *mime_data = qobject_cast<const LibraryTreeItemsMime*>(data);
	if(mime_data == NULL)
		return false;
	const std::vector<SafePointer<LibraryTreeItem>> &items = mime_data->GetItems();

	bool contains_libraries = false, contains_drawings = false;
	for(size_t i = 0; i < items.size(); ++i) {
		LibraryTreeItem *item = items[i].Get();
		if(item != NULL) {
			switch(item->GetTreeItemType()) {
				case LIBRARYTREEITEMTYPE_LIBRARY: contains_libraries = true; break;
				case LIBRARYTREEITEMTYPE_DRAWING: contains_drawings = true; break;
			}
		}
	}

	// library drop mode
	if(contains_libraries) {

		// is parent the root?
		if(!parent.isValid()) {
			return true;
		}

		// What type of parent is this?
		LibraryTreeItem *parent_ptr = (LibraryTreeItem*) parent.internalPointer();
		switch(parent_ptr->GetTreeItemType()) {
			case LIBRARYTREEITEMTYPE_LIBRARY: {
				Library *library = static_cast<Library*>(parent_ptr);
				size_t index = IndexInVector(m_libraries, library);
				row = (location <= DROPLOCATION_ON_BEFORE)? (int) index : (int) (index + 1);
				column = 0;
				parent = QModelIndex();
				return true;
			}
			case LIBRARYTREEITEMTYPE_DRAWING: {
				Drawing *drawing = static_cast<Drawing*>(parent_ptr);
				Library *library = drawing->GetParent();
				size_t index = IndexInVector(m_libraries, library);
				row = (int) (index + 1);
				column = 0;
				parent = QModelIndex();
				return true;
			}
		}

	}

	// drawing drop mode
	if(contains_drawings) {

		return true;
	}

	return false;
}

bool LibraryManager::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	UNUSED(column);

	if(action == Qt::IgnoreAction)
		return true;

	if(!data->hasFormat("application/x-alterpcb-librarytreeitem"))
		return false;
	const LibraryTreeItemsMime *mime_data = qobject_cast<const LibraryTreeItemsMime*>(data);
	if(mime_data == NULL)
		return false;
	const std::vector<SafePointer<LibraryTreeItem>> &items = mime_data->GetItems();

	// is parent the root?
	if(!parent.isValid()) {

		layoutAboutToBeChanged();

		// extract all libraries
		std::vector<TrackingPointer<Library>> libraries;
		for(size_t i = 0; i < items.size(); ++i) {
			LibraryTreeItem *item = items[i].Get();
			if(item != NULL && item->GetTreeItemType() == LIBRARYTREEITEMTYPE_LIBRARY) {
				Library *library = static_cast<Library*>(item);
				size_t index = IndexInVector(m_libraries, library);
				libraries.emplace_back(std::move(m_libraries[index]));
			}
		}

		// insert them in the correct position
		size_t target_index = ((size_t) row < GetLibraryCount()) ? row : GetLibraryCount();
		BulkInsertIntoVector(m_libraries, libraries, target_index);

		UpdatePersistentModelIndices();
		layoutChanged();

		return true;
	}

	// What type of parent is this?
	LibraryTreeItem *parent_ptr = (LibraryTreeItem*) parent.internalPointer();
	switch(parent_ptr->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY: {
			Library *library = static_cast<Library*>(parent_ptr);
			UNUSED(library);
			return false;
		}
		case LIBRARYTREEITEMTYPE_DRAWING: {
			return false;
		}
	}

	// this should never be reached
	assert(false);
	return false;

}

void LibraryManager::UpdatePersistentModelIndices() {

	// Qt exposes persistent model indices as a flat list, so lookup is pretty slow. Our own internal structure is much
	// more efficient. So instead of trying to figure out which persistent indices have been affected by recent moves,
	// it's much faster and easier to just regenerate all of them based on our own internal structure.
	QModelIndexList oldlist = persistentIndexList();
	QModelIndexList newlist = oldlist;
	for(int i = 0; i < oldlist.size(); ++i) {
		int row = 0; // to avoid compiler warning
		LibraryTreeItem *item_ptr = (LibraryTreeItem*) oldlist[i].internalPointer();
		switch(item_ptr->GetTreeItemType()) {
			case LIBRARYTREEITEMTYPE_LIBRARY: {
				Library *library = static_cast<Library*>(item_ptr);
				row = (int) IndexInVector(m_libraries, library);
				break;
			}
			case LIBRARYTREEITEMTYPE_DRAWING: {
				Drawing *drawing = static_cast<Drawing*>(item_ptr);
				Library *library = drawing->GetParent();
				row = (int) library->GetDrawingIndex(drawing);
				break;
			}
		}
		if(row != oldlist[i].row()) {
			newlist[i] = createIndex(row, oldlist[i].column(), item_ptr);
		}
	}

	// presumably Qt optimizes this internally
	changePersistentIndexList(oldlist, newlist);

}
