#include "LayerViewer.h"
#include "LayerManager.h"
#include <iostream>
#include "LayerManager.h"
#include "MainWindow.h"

LayerViewer::LayerViewer(QWidget *parent, MainWindow *main_window) : QTreeView(parent) {
	setModel(main_window->GetLayerManager());
	setUniformRowHeights(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setRootIsDecorated(false);
	setFocusPolicy(Qt::NoFocus);
	header()->setMovable(false);
	header()->setStretchLastSection(false);
	header()->setResizeMode(0,QHeaderView::Stretch);
	header()->setResizeMode(1,QHeaderView::Fixed);
	header()->setResizeMode(2,QHeaderView::Fixed);
	setColumnWidth(1,style()->pixelMetric(QStyle::PM_IndicatorWidth)+8);
	setColumnWidth(2,style()->pixelMetric(QStyle::PM_IndicatorWidth)+8);
	header()->setDefaultAlignment(Qt::AlignHCenter);
}

void LayerViewer::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	if(selected.indexes().size() == 1){
		static_cast<LayerManager*>(model())->activeLayerChanged(selected.indexes()[0]);
	}
	QAbstractItemView::selectionChanged(selected, deselected);
}
