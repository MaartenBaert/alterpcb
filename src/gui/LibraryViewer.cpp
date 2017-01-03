#include "LibraryViewer.h"
#include <iostream>
#include "LibraryTreeItem.h"

LibraryViewer::LibraryViewer(QWidget *parent) : QTreeView(parent) {

	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	//setDragDropOverwriteMode(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	setHeaderHidden(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setUniformRowHeights(true);

	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnRightClick(QPoint)));
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnDoubleClick(QModelIndex)));

}

LibraryViewer::~LibraryViewer() {

}

void LibraryViewer::OnDoubleClick(const QModelIndex &index)
{
	UNUSED(index);
	std::cerr << "LIBVIEWER: TODO DoubleClick open file" << std::endl;
}

void LibraryViewer::OnRightClick(const QPoint &point)
{
	UNUSED(point);
	 QMenu *menu = new QMenu;
	 QModelIndex index = this->currentIndex();

	 LibraryTreeItem *Item = static_cast<LibraryTreeItem*>(index.internalPointer());
	 switch(Item->GetTreeItemType()) {
		 case LIBRARYTREEITEMTYPE_LIBRARY : {
			 std::cerr << "LIBVIEWER: TODO RightCLick on lib" << std::endl;
			 menu->addAction(QString("Close"));
			 menu->addAction(QString("Rename"));
			 menu->addAction( tr("Delete"));
			 break;
		 }
		 case LIBRARYTREEITEMTYPE_DRAWING : {
			  std::cerr << "LIBVIEWER: TODO RightCLick on drawing" << std::endl;
			  menu->addAction(tr("Open"));
			  menu->addAction(tr("Rename"));
			  menu->addAction(tr("Cut"));
			  menu->addAction(tr("Copy"));
			  menu->addAction(tr("Delete"));
			  break;
		 }
		 default : {
			break;
		 }
	 }
	 menu->exec(QCursor::pos());

}

