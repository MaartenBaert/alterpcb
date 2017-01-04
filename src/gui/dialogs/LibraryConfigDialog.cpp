#include "LibraryConfigDialog.h"
#include <LibraryManager.h>
#include "LibraryViewer.h"
#include "LibraryManagerConfigProxy.h"



LibraryConfigDialog::LibraryConfigDialog(LibraryManager* library_manager)
{
	setWindowTitle("Library Manager");

	QTreeView *library_viewer;
	library_viewer = new QTreeView(this);
	LibraryManagerConfigProxy *proxy_model;
	proxy_model = new LibraryManagerConfigProxy(library_viewer);
	proxy_model->setSourceModel(library_manager);
	library_viewer->setModel(proxy_model);
	library_viewer->setMinimumSize(500, 400);
	library_viewer->setContentsMargins(0,0,0,0);
	library_viewer->setColumnWidth(0, 150);

	QPushButton *button_add = new QPushButton(tr("Add Library"));
	QPushButton *button_remove = new QPushButton(tr("Remove Library"));

	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->setSpacing(0);
	layout1->setContentsMargins(0,0,0,0);
	QVBoxLayout *layout2 = new QVBoxLayout();
	layout2->setAlignment(Qt::AlignTop);
	layout2->setContentsMargins(10,10,10,10);
	layout1->addWidget(library_viewer);
	layout2->addWidget(button_add);
	layout2->addWidget(button_remove);
	layout1->addItem(layout2);
	setLayout(layout1);

}
