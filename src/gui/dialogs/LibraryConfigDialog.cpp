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

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(library_viewer);
	setLayout(layout);

}
