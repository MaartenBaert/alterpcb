/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MainWindow.h"
#include "LibraryManager.h"
#include "LibraryViewer.h"
#include "ParameterViewer.h"
#include "LayerManager.h"
#include "LayerViewer.h"

#include "DrawingViewer.h"
#include "dialogs/LibraryConfigDialog.h"
#include "dialogs/LayerConfigDialog.h"

const QString MainWindow::WINDOW_TITLE = "AlterPCB";

MainWindow::MainWindow(LibraryManager* library_manager,LayerManager* layer_manager) {

	setWindowTitle(WINDOW_TITLE);
	m_library_manager = library_manager;
	m_layer_manager = layer_manager;
	m_library_config_dialog = NULL;
	m_layer_config_dialog = NULL;

	DocumentEditor *editor = new DocumentEditor(this,library_manager,layer_manager);

	QMenuBar *menubar = new QMenuBar(this);
	{
		QMenu *menu_file = menubar->addMenu(tr("&File"));
		menu_file->addAction(tr("&Open"));
		menu_file->addAction(tr("&Save"));
		menu_file->addAction(tr("S&ave as"));
		menu_file->addSeparator();
		menu_file->addAction(tr("E&xport"));
		menu_file->addSeparator();
		menu_file->addAction(tr("&Close"));
		menu_file->addAction(tr("C&lose all"));
		menu_file->addSeparator();
		menu_file->addAction(tr("&Exit"));
	}
	{
		QMenu *menu_edit = menubar->addMenu(tr("&Edit"));
		QAction *act_open_library_manager = menu_edit->addAction(tr("Library Manager"));
		connect(act_open_library_manager, SIGNAL (triggered(bool)), this, SLOT (OpenLibraryConfigDialog()));
		QAction *act_open_layer_manager = menu_edit->addAction(tr("Layer Manager"));
		connect(act_open_layer_manager, SIGNAL (triggered(bool)), this, SLOT (OpenLayerConfigDialog()));
		menu_edit->addAction("Test");
	}

	QMenu *menu_view = menubar->addMenu(tr("&View"));
	QMenu *menu_view_showhide = menu_view->addMenu(tr("Show/Hide"));
	QAction *menu_view_restore = menu_view->addAction(tr("&Restore"));
	UNUSED(menu_view_restore);

	setMenuBar(menubar);

	ParameterViewer *parameter_viewer;
	{
		QDockWidget *dock = new QDockWidget(tr("Parameters"), this);
		dock->setObjectName("dock_parameters");
		parameter_viewer = new ParameterViewer(dock);
		dock->setWidget(parameter_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::RightDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	LayerViewer *layer_viewer;
	{
		QDockWidget *dock = new QDockWidget(tr("Layers"), this);
		dock->setObjectName("dock_layers");
		layer_viewer = new LayerViewer(dock,m_layer_manager);
		dock->setWidget(layer_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	QTreeView *library_viewer;
	{
		QDockWidget *dock = new QDockWidget(tr("Libraries"), this);
		dock->setObjectName("dock_libraries");
		library_viewer = new LibraryViewer(dock);
		library_viewer->setModel(library_manager);
		library_viewer->hideColumn(1); //TODO remove once the searchable lib viewer is made with a proxy model (IF this is not there, the drag and drop is broken)
		dock->setWidget(library_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	setCentralWidget(editor);

	QToolBar *toolbar = new QToolBar(this);
	toolbar->setMovable(false);
	toolbar->addAction(QIcon::fromTheme("document-open"), tr("Open"));
	toolbar->addAction(QIcon::fromTheme("document-save"), tr("Save"));
	toolbar->addAction(QIcon::fromTheme("document-save-as"), tr("Save as"));

	toolbar->addSeparator();
	toolbar->addAction(QIcon::fromTheme("edit-cut"), tr("Cut"));
	toolbar->addAction(QIcon::fromTheme("edit-copy"), tr("Copy"));
	toolbar->addAction(QIcon::fromTheme("edit-paste"), tr("Paste"));
	toolbar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
	toolbar->addAction(QIcon::fromTheme("edit-select-all"), tr("Select all"));

	toolbar->addSeparator();
	toolbar->addAction(QIcon::fromTheme("edit-undo"), tr("Undo"));
	toolbar->addAction(QIcon::fromTheme("edit-redo"), tr("Redo"));

	toolbar->addSeparator();
	toolbar->addAction(QIcon::fromTheme("zoom-fit-best"), tr("Zoom fit"));
	toolbar->addAction(QIcon::fromTheme("zoom-in"), tr("Zoom in"));
	toolbar->addAction(QIcon::fromTheme("zoom-out"), tr("Zoom out"));

	addToolBar(toolbar);

	showMaximized();
}

MainWindow::~MainWindow() {
	CloseLibraryConfigDialog();
}

void MainWindow::OpenLibraryConfigDialog()
{
	if(m_library_config_dialog == NULL) {
		m_library_config_dialog = new LibraryConfigDialog(this,m_library_manager);
		m_library_config_dialog->show();
	} else {
		m_library_config_dialog->raise();
	}
}

void MainWindow::CloseLibraryConfigDialog()
{
	if(m_library_config_dialog != NULL) {
		m_library_config_dialog->deleteLater();
		m_library_config_dialog = NULL;
	}
}

void MainWindow::OpenLayerConfigDialog()
{
	if(m_layer_config_dialog == NULL) {
		m_layer_config_dialog = new LayerConfigDialog(this,m_layer_manager);
		m_layer_config_dialog->show();
	} else {
		m_layer_config_dialog->raise();
	}
}

void MainWindow::CloseLayerConfigDialog()
{
	if(m_layer_config_dialog != NULL) {
		m_layer_config_dialog->deleteLater();
		m_layer_config_dialog = NULL;
	}
}
