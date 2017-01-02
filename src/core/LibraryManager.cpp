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
#include "Drawing.h"
#include "StringRegistry.h"
#include <iostream>

LibraryManager::LibraryManager() {
	// nothing
}

LibraryManager::~LibraryManager() {
	// nothing
}

Library *LibraryManager::NewLibrary(const std::string &name, const std::string &filename, LibraryType type) {
	ForwardPointer<Library> library(new Library(this, name, filename, type));
	Library *ptr = library.Get();
	m_libraries.emplace_back(std::move(library));

	return ptr;
}

void LibraryManager::DeleteLibrary(Library *library) {
	DeleteFromVector(m_libraries, library);
}


QModelIndex LibraryManager::index(int row, int column, const QModelIndex &parent) const {

	// Check whether the requested index (i.e. row and column) is valid. We can already check the things that don't
	// depend on the type of item we are working with. The final row count check will be done later.
	if(row < 0 || column != 0)
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
			return createIndex(IndexInVector(m_libraries, library), 0, static_cast<LibraryTreeItem*>(library));
		}
	}

	// this should never be reached
	assert(false);
	return QModelIndex();

}

int LibraryManager::rowCount(const QModelIndex &parent) const {

	// is parent the root?
	if(!parent.isValid())
		return GetLibraryCount();

	// what type of parent is this?
	LibraryTreeItem *parent_ptr = (LibraryTreeItem*) parent.internalPointer();
	switch(parent_ptr->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY: {
			Library *library = static_cast<Library*>(parent_ptr);
			return library->GetLayoutCount();
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
	return 1;
}

Qt::DropActions LibraryManager::supportedDropActions() const {
	return Qt::MoveAction;
}

Qt::ItemFlags LibraryManager::flags(const QModelIndex &index) const {
	if(!index.isValid())
		return Qt::ItemIsDropEnabled; // dropping on root is allowed
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
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

bool LibraryManager::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	UNUSED(column);

	if(action == Qt::IgnoreAction)
		return true;

	const LibraryTreeItemsMime *mime_data = qobject_cast<const LibraryTreeItemsMime*>(data);
	if(mime_data == NULL)
		return false;
	const std::vector<SafePointer<LibraryTreeItem>> &items = mime_data->GetItems();

	// is parent the root?
	if(!parent.isValid()) {

		layoutAboutToBeChanged();

		// extract all libraries
		std::vector<ForwardPointer<Library>> libraries;
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

QVariant LibraryManager::data(const QModelIndex &index, int role) const {

	if(!index.isValid())
		return QVariant();

	if(role != Qt::DisplayRole)
		return QVariant();

	LibraryTreeItem *item_ptr = (LibraryTreeItem*) index.internalPointer();
	switch(item_ptr->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY: {
			Library *library = static_cast<Library*>(item_ptr);
			return QString::fromStdString(library->GetName());
		}
		case LIBRARYTREEITEMTYPE_DRAWING: {
			Drawing *drawing = static_cast<Drawing*>(item_ptr);
			return QString::fromStdString(StringRegistry::GetString(drawing->GetName()));
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
