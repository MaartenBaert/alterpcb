#include "LibraryManagerConfigProxy.h"
#include "LibraryTreeItem.h"
#include "Library.h"
#include "LibraryManager.h"
#include "Drawing.h"
#include <iostream>


LibraryManagerConfigProxy::LibraryManagerConfigProxy(QObject *parent) : QAbstractProxyModel(parent) {
	// nothing
}

LibraryManagerConfigProxy::~LibraryManagerConfigProxy()
{

}

QModelIndex LibraryManagerConfigProxy::index(int row, int column, const QModelIndex &parent) const {
	UNUSED(parent);
	return createIndex(row,column);

}

QModelIndex LibraryManagerConfigProxy::parent(const QModelIndex &index) const {
	UNUSED(index);
	return QModelIndex();
}

int LibraryManagerConfigProxy::rowCount(const QModelIndex &parent) const {
	if(parent.isValid())
		return 0;

	return (static_cast<LibraryManager*>(sourceModel()))->GetLibraryCount();
}

int LibraryManagerConfigProxy::columnCount(const QModelIndex &parent) const {
	UNUSED(parent);
	return 2;
}

QModelIndex LibraryManagerConfigProxy::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!sourceModel())
		return QModelIndex();

	if (!proxyIndex.isValid())
		return QModelIndex();

	return sourceModel()->index(proxyIndex.row(),0,QModelIndex());
}

QModelIndex LibraryManagerConfigProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceModel())
		return QModelIndex();

	if (!sourceIndex.isValid())
		return QModelIndex();

	if (!sourceIndex.parent().isValid())
		return QModelIndex();

	return index(sourceIndex.row(),sourceIndex.column());
}

QVariant LibraryManagerConfigProxy::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(role == Qt::DisplayRole) {
		LibraryTreeItem *item_ptr = (LibraryTreeItem*) mapToSource(index).internalPointer();
		switch(item_ptr->GetTreeItemType()) {
			case LIBRARYTREEITEMTYPE_LIBRARY: {
				Library *library = static_cast<Library*>(item_ptr);
				if(index.column() == 0) {
					return QString::fromStdString(library->GetName());
				}
				else if (index.column() == 1) {
					return QString::fromStdString(library->GetFileName());
				}
			}
			default : {
				return QVariant();
			}}
	}
	else {
		return QVariant();
	}

	// this should never be reached
	assert(false);
	return QVariant();
}
