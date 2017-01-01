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


}

LibraryManager::~LibraryManager() {

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


QModelIndex LibraryManager::index(int row, int column, const QModelIndex &parent)
const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	LibraryTreeItem *parentItem;

	if (parent.isValid()){
		parentItem = static_cast<LibraryTreeItem*>(parent.internalPointer());
		// check which type the parent is
		switch(parentItem->GetTreeItemType()) {
			case LIBRARYTREEITEMTYPE_LIBRARY : {
				Library *libraryItem;
				libraryItem = static_cast<Library*>(parentItem);
				if((size_t) row <= libraryItem->GetLayoutCount())
					return createIndex(row, column, static_cast<LibraryTreeItem*>(libraryItem->GetLayout(row)));
				else
					return QModelIndex();
			}
			default : {
				return QModelIndex();
			}

		}
	}
	else
	{
		if((size_t) row <= GetLibraryCount()) {
			return createIndex(row, column, GetLibrary(row));
		}
		else {
			return QModelIndex();
		}
	}
}

QModelIndex LibraryManager::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	LibraryTreeItem *childItem = static_cast<LibraryTreeItem*>(index.internalPointer());
	switch(childItem->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY : {
			// parent is root
			return QModelIndex();
		}
		case LIBRARYTREEITEMTYPE_DRAWING : {
			// parent is a library
			Drawing *drawingItem;
			drawingItem = static_cast<Drawing*>(childItem);
			Library *libraryItem = drawingItem->GetParent();
			return createIndex(IndexInVector(m_libraries,libraryItem), 0, static_cast<LibraryTreeItem*>(libraryItem));
		}
		default : {
			return QModelIndex();
		}
	}

}

int LibraryManager::rowCount(const QModelIndex &parent) const
{
	LibraryTreeItem *parentItem;
	if (!parent.isValid()) {
		// parent is root
		return GetLibraryCount();
	}
	else {
		parentItem = static_cast<LibraryTreeItem*>(parent.internalPointer());
		switch(parentItem->GetTreeItemType()) {
			case LIBRARYTREEITEMTYPE_LIBRARY : {
				Library *libraryItem;
				libraryItem = static_cast<Library*>(parentItem);
				return libraryItem->GetLayoutCount();
			}
			default : {
				return 0;
			}

		}
	}
}

int LibraryManager::columnCount(const QModelIndex &parent) const
{
	UNUSED(parent);
	return 1;
}

Qt::DropActions LibraryManager::supportedDropActions() const
{
	return Qt::MoveAction | Qt::MoveAction;
}

Qt::ItemFlags LibraryManager::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

	if (!index.isValid()){
		return Qt::ItemIsDropEnabled| defaultFlags;
	}
	else {
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled| defaultFlags;
	}


}

QStringList LibraryManager::mimeTypes() const
{
	QStringList types;
	types << "application/vnd.text.list";
	return types;
}

QMimeData *LibraryManager::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QString text;
	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			// send the address of the dragged item + type (so that we know how to cast it later)
			LibraryTreeItem *item = static_cast<LibraryTreeItem*>(index.internalPointer());
			switch(item->GetTreeItemType()) {
				case LIBRARYTREEITEMTYPE_LIBRARY : {
					Library *libraryItem;
					libraryItem = static_cast<Library*>(item);
					text = "LIB_" + QString::number((qint64) libraryItem);
					break;
				}
				case LIBRARYTREEITEMTYPE_DRAWING : {
					Drawing *drawingItem;
					drawingItem = static_cast<Drawing*>(item);
					text = text = "DRA_" + QString::number((qint64) drawingItem);
					break;
				}
				default : {
					text = QString("ERR");
					break;
				}
			}

			stream << text;
		}
	}

	mimeData->setData("application/vnd.text.list", encodedData);
	return mimeData;
}

bool LibraryManager::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("application/vnd.text.list"))
		return false;

	if (column > 0)
		return false;

	QByteArray encodedData = data->data("application/vnd.text.list");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QStringList newItems;
	while (!stream.atEnd()) {
		QString text;
		stream >> text;
		newItems << text;
	}

	foreach (QString text, newItems) {
		QStringList data =  text.split("_");
		QString objecttype = data.at(0);
		QString pointerstr = data.at(1);
		long objectptr = pointerstr.toLong();

		if(objecttype == QString("LIB")) {
			Library * libraryItem;
			libraryItem = reinterpret_cast<Library*>(objectptr);

			// Library can only be placed between other library OR at the end
			if(row<0 && parent.row()>0){
				return false;
			}

			// move the lib
			if(row<0 && parent.row()<0) {
				// place at the end
				std::cerr << std::to_string(m_libraries.size()) << std::endl;
				moveInVector(m_libraries,IndexInVector(m_libraries,libraryItem),m_libraries.size());
			}
			else {
				// place in the middle
				moveInVector(m_libraries,IndexInVector(m_libraries,libraryItem),row);
			}
		}
		else if(objecttype == QString("DRA")) {
			Drawing *drawingItem;
			drawingItem = reinterpret_cast<Drawing*>(objectptr);

			// Drawing can only be placed between other drawings OR ontop of a library
			if(parent.row()<0)
			{
				return false;
			}
			LibraryTreeItem *parentItem = static_cast<LibraryTreeItem*>(parent.internalPointer());
			if(parentItem->GetTreeItemType() == LIBRARYTREEITEMTYPE_DRAWING){
				return false;
			}

			// move the drawing
			Library * targetlibraryItem;
			targetlibraryItem = static_cast<Library*>(parentItem);

			if(targetlibraryItem == drawingItem->GetParent()) {
				// Move within the same library
				targetlibraryItem->MovePositionDrawing(drawingItem , row);
			}
			else {
				if(row < 0) {
					// Move to other library (if dragged ontop of other lib)
					ForwardPointer<Drawing> drawingItemPtr = drawingItem->GetParent()->MoveOutDrawing(drawingItem);
					targetlibraryItem->MoveInDrawing(&drawingItemPtr);
				}
				else {
					// Move to target index in other library
					ForwardPointer<Drawing> drawingItemPtr = drawingItem->GetParent()->MoveOutDrawing(drawingItem);
					targetlibraryItem->MoveInDrawing(&drawingItemPtr);
					targetlibraryItem->MovePositionDrawing(drawingItem , row);
				}
			}
		}
	}

	layoutChanged();
	return true;
}

QVariant LibraryManager::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	LibraryTreeItem *item = static_cast<LibraryTreeItem*>(index.internalPointer());
	switch(item->GetTreeItemType()) {
		case LIBRARYTREEITEMTYPE_LIBRARY : {
			Library *libraryItem;
			libraryItem = static_cast<Library*>(item);
			return QString::fromStdString(libraryItem->GetName());

		}
		case LIBRARYTREEITEMTYPE_DRAWING : {
			Drawing *drawingItem;
			drawingItem = static_cast<Drawing*>(item);
			return QString::fromStdString(StringRegistry::GetString(drawingItem->GetName()));
		}
		default : {
			return QString("inplementation error");
		}
	}
}


QVariant LibraryManager::headerData(int section, Qt::Orientation orientation,int role) const
{
	UNUSED(section);
	UNUSED(orientation);
	UNUSED(role);
	return QVariant();
}

