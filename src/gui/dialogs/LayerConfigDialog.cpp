#include "LayerConfigDialog.h"
#include "MainWindow.h"
#include "LayerWidget.h"
#include "LayerManager.h"
#include <iostream>

LayerConfigDialog::LayerConfigDialog(MainWindow *parent ,LayerManager *layer_manager) : QDialog(parent)
{
	m_parent = parent;
	setWindowTitle(tr("Layer Manager"));

	LayerWidget *layer_widget = new LayerWidget(this,layer_manager);
	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->setSpacing(0);
	layout1->setContentsMargins(0,0,0,0);
	layout1->addWidget(layer_widget);
	setLayout(layout1);

	setMinimumSize(500,500);
}

void LayerConfigDialog::closeEvent(QCloseEvent *event)
{
	m_parent->CloseLayerConfigDialog();
	event->accept();
}
