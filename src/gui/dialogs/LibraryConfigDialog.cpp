#include "LibraryConfigDialog.h"
#include <LibraryManager.h>
#include "LibraryViewer.h"
#include "LibraryManagerConfigProxy.h"
#include "gui/components/FilepathDelegate.h"
#include "MainWindow.h"


LibraryConfigDialog::LibraryConfigDialog(MainWindow *parent, LibraryManager* library_manager) : QDialog(parent)
{
	m_parent = parent;
	setWindowTitle(tr("Library Manager"));

	m_library_viewer = new QTreeView(this);
	LibraryManagerConfigProxy *proxy_model;
	proxy_model = new LibraryManagerConfigProxy(m_library_viewer);
	proxy_model->setSourceModel(library_manager);
	m_library_viewer->setModel(proxy_model);
	m_library_viewer->setMinimumSize(500, 400);
	m_library_viewer->setContentsMargins(0,0,0,0);
	m_library_viewer->setColumnWidth(0, 150);
	m_library_viewer->setRootIsDecorated(false);

	m_library_viewer->setItemDelegateForColumn(1,&m_delegate);

	QPushButton *button_add = new QPushButton(tr("Add Library"));
	QPushButton *button_remove = new QPushButton(tr("Remove Library"));
	connect(button_add, SIGNAL (clicked(bool)), this, SLOT (OnAddLibrary()));
	connect(button_remove, SIGNAL (clicked(bool)), this, SLOT (OnRemoveLibrary()));

	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->setSpacing(0);
	layout1->setContentsMargins(0,0,0,0);
	QVBoxLayout *layout2 = new QVBoxLayout();
	layout2->setAlignment(Qt::AlignTop);
	layout2->setContentsMargins(10,10,10,10);
	layout1->addWidget(m_library_viewer);
	layout2->addWidget(button_add);
	layout2->addWidget(button_remove);
	layout1->addItem(layout2);
	setLayout(layout1);

}

void LibraryConfigDialog::closeEvent(QCloseEvent *event)
{
	m_parent->CloseLibraryConfigDialog();
	event->accept();
}

void LibraryConfigDialog::OnAddLibrary()
{
	m_parent->GetLibraryManager()->NewLibrary("New Library","", LIBRARYTYPE_JSON);
}

void LibraryConfigDialog::OnRemoveLibrary()
{
	int index = m_library_viewer->currentIndex().row();
	if(index >= 0 && index < m_parent->GetLibraryManager()->GetLibraryCount()){
		m_parent->GetLibraryManager()->DeleteLibrary(m_parent->GetLibraryManager()->GetLibrary(index));
	}
}
