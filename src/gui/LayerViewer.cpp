#include "LayerViewer.h"
#include "LayerManager.h"
#include <iostream>
#include "LayerManager.h"
#include "MainWindow.h"

LayerViewer::LayerViewer(MainWindow *main_window, QWidget *parent) : QTreeView(parent) {

	m_main_window = main_window;

	setModel(main_window->GetLayerManager());
	setUniformRowHeights(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setRootIsDecorated(false);
	setFocusPolicy(Qt::NoFocus);

	// set up treeview columns
	header()->setDefaultAlignment(Qt::AlignHCenter);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
	header()->setSectionResizeMode(1, QHeaderView::Fixed);
	header()->setSectionResizeMode(2, QHeaderView::Fixed);
	header()->setSectionsMovable(false);
	header()->setStretchLastSection(false);
	setColumnWidth(1, style()->pixelMetric(QStyle::PM_IndicatorWidth) + 8);
	setColumnWidth(2, style()->pixelMetric(QStyle::PM_IndicatorWidth) + 8);

}

LayerViewer::~LayerViewer() {
	// nothing
}

void LayerViewer::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
	if(selected.indexes().size() == 1) {
		m_main_window->GetLayerManager()->activeLayerChanged(selected.indexes()[0]);
	}
	QAbstractItemView::selectionChanged(selected, deselected);
}
