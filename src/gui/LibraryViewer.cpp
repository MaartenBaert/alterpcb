#include "LibraryViewer.h"
#include <iostream>
#include "LibraryTreeItem.h"
#include "LibraryManager.h"
#include "core/CoreBasics.h"
#include "MainWindow.h"
#include "Drawing.h"

LibraryViewer::LibraryViewer(QWidget *parent, MainWindow *main_window) : QTreeView(parent) {
	m_main_window = main_window;

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

	LibraryTreeItem *Item = (LibraryTreeItem*) index.internalPointer();
	if(Item->GetTreeItemType() == LIBRARYTREEITEMTYPE_DRAWING){
		m_main_window->GetDocumentViewer()->OpenDrawing(static_cast<Drawing*>(Item));
	}
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
			menu->addAction( tr("Add layout"));
			menu->addAction( tr("Add Schematic"));
			menu->addAction( tr("Add Symbol"));
			menu->addSeparator();
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

DropLocation LibraryViewer::getDropLocation(QRect &index_rect, QPoint pos) {
	if(pos.y() < index_rect.y() + index_rect.height() / 2) {
		if(pos.y() < index_rect.y() + index_rect.height() / 4) {
			return DROPLOCATION_BEFORE;
		} else {
			return DROPLOCATION_ON_BEFORE;
		}
	} else {
		if(pos.y() < index_rect.y() + index_rect.height() * 3 / 4) {
			return DROPLOCATION_ON_AFTER;
		} else {
			return DROPLOCATION_AFTER;
		}
	}
}

void LibraryViewer::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasFormat("application/LibraryTreeItemsMime"))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

void LibraryViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
	m_drop_indicator_rect = QRect();
	viewport()->update();
	event->ignore();
}

void LibraryViewer::dragMoveEvent(QDragMoveEvent *event)
{

	QModelIndex index = indexAt(event->pos());
	QRect index_rect = visualRect(index);

	// where is the mouse compaired to the index_rect
	DropLocation location_indicator = getDropLocation(index_rect,event->pos());
	int row = -1, column = -1;
	QModelIndex parent = index;
	if(static_cast<LibraryManager*>(this->model())->dropLocation(event->mimeData(), event->dropAction(), location_indicator, row, column, parent)) {
		if(row < 0 || column < 0) {
			m_drop_indicator_rect = visualRect(parent);
		} else {
			int numrows =  model()->rowCount(parent);
			if(numrows == 0) {
				m_drop_indicator_rect = visualRect(parent);
			} else if(row < numrows) {
				QRect index_rect = visualRect(model()->index(row, column, parent));
				m_drop_indicator_rect = QRect(index_rect.x(), index_rect.y() - 1, index_rect.width(), 2);
			} else {
				QRect index_rect = visualRect(model()->index(numrows - 1, column, parent));
				m_drop_indicator_rect = QRect(index_rect.x(), index_rect.y() + index_rect.height() - 1, index_rect.width(), 2);
			}
		}
		viewport()->update();
		event->accept();
	}
	else {
		m_drop_indicator_rect = QRect();
		event->ignore();
	}

}

void LibraryViewer::dropEvent(QDropEvent *event)
{
	m_drop_indicator_rect = QRect();
	QModelIndex index = indexAt(event->pos());
	QRect index_rect = visualRect(index);

	// where is the mouse compaired to the index_rect
	DropLocation location_indicator = getDropLocation(index_rect,event->pos());
	int row = -1, column = -1;
	QModelIndex parent = index;
	if((static_cast<LibraryManager*>(this->model())->dropLocation(event->mimeData(), event->dropAction(), location_indicator, row, column, parent))) {
		if (static_cast<LibraryManager*>(this->model())->dropMimeData(event->mimeData(), dragDropMode() == InternalMove ? Qt::MoveAction : event->dropAction(), row, column, parent)) {
			if (dragDropMode() == InternalMove) {
				event->setDropAction(Qt::MoveAction);
			}
			event->accept();
		}
	}
	stopAutoScroll();
	setState(NoState);
	viewport()->update();
}

void LibraryViewer::paintEvent(QPaintEvent *event)
{
	QPainter painter(viewport());
	drawTree(&painter, event->region());
	if(!m_drop_indicator_rect.isNull()) {
		painter.setPen(QColor(0,0,0));
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(m_drop_indicator_rect.adjusted(0,0,-1,-1));
	}
}


