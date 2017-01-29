#include "LayerViewer.h"
#include "LayerManager.h"
#include <iostream>

LayerViewer::LayerViewer(QWidget *parent) : QTreeView(parent) {
	setModel(new LayerManager());
	setUniformRowHeights(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setRootIsDecorated(false);
	setFocusPolicy(Qt::NoFocus);
	header()->setMovable(false);
	header()->setStretchLastSection(false);
	header()->setResizeMode(0,QHeaderView::Stretch);
	header()->setResizeMode(1,QHeaderView::Fixed);
	header()->setResizeMode(2,QHeaderView::Fixed);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
}

void LayerViewer::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	if(selected.indexes().size() == 1){
		static_cast<LayerManager*>(model())->activeLayerChanged(selected.indexes()[0]);
	}
	QAbstractItemView::selectionChanged(selected, deselected);
}
