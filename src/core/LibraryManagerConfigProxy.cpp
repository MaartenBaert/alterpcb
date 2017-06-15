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

void LibraryManagerConfigProxy::setSourceModel(QAbstractItemModel *model)
{
	QAbstractProxyModel::setSourceModel(model);
	connect(sourceModel(),SIGNAL(layoutChanged()),this,SLOT(onLayoutChanged()));
	connect(sourceModel(),SIGNAL(layoutAboutToBeChanged()),this,SLOT(onLayoutAboutToBeChanged()));
	connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
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

	return (int) (static_cast<LibraryManager*>(sourceModel()))->GetLibraryCount();
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

	return sourceModel()->index(proxyIndex.row(),proxyIndex.column(),QModelIndex());
}

QModelIndex LibraryManagerConfigProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceModel())
		return QModelIndex();

	if (!sourceIndex.isValid())
		return QModelIndex();

	return index(sourceIndex.row(),sourceIndex.column());
}

QVariant LibraryManagerConfigProxy::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(role == Qt::DisplayRole || role == Qt::EditRole || role ==  Qt::ForegroundRole) {
		return sourceModel()->data(mapToSource(index),role);
	}
	else {
		return QVariant();
	}

	// this should never be reached
	assert(false);
	return QVariant();
}

QVariant LibraryManagerConfigProxy::headerData(int section, Qt::Orientation orientation,int role) const {
	UNUSED(orientation);
	if (role != Qt::DisplayRole)
		return QVariant();
	if(section == 0) {
		return QString(tr("Name"));
	}
	else if(section == 1) {
		return QString(tr("File Path"));
	}
	else {
		return QVariant();
	}
}

Qt::ItemFlags LibraryManagerConfigProxy::flags(const QModelIndex &index) const
{
	if (!index.isValid()){
		return 0;
	}

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool LibraryManagerConfigProxy::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(role ==  Qt::EditRole) {
		return sourceModel()->setData(mapToSource(index),value,role);
	}

	return false;
}

void LibraryManagerConfigProxy::onLayoutChanged()
{
	emit layoutChanged();
}

void LibraryManagerConfigProxy::onLayoutAboutToBeChanged()
{
	emit layoutAboutToBeChanged();
}

void LibraryManagerConfigProxy::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	emit dataChanged(mapFromSource(topLeft),mapFromSource(bottomRight));
}
