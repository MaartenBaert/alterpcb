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
#include "Drawing.h"

#include "DrawingViewer.h"
#include "dialogs/LibraryConfigDialog.h"
#include "dialogs/LayerConfigDialog.h"
#include "dialogs/GerberImportDialog.h"

const QString MainWindow::WINDOW_TITLE = "AlterPCB";

MainWindow::MainWindow(LibraryManager* library_manager) {
	setWindowTitle(WINDOW_TITLE);
	m_library_manager = library_manager;
	m_layer_manager = new LayerManager(this);

	m_library_config_dialog = NULL;
	m_layer_config_dialog = NULL;
	m_import_gerber_dialog = NULL;

	QMenuBar *menubar = new QMenuBar(this);
	{
		QMenu *menu_file = menubar->addMenu(tr("&File"));
		menu_file->addAction(tr("&Open"));
		menu_file->addAction(tr("&Save"));
		menu_file->addAction(tr("S&ave as"));
		menu_file->addSeparator();
		QMenu *export_menu = menu_file->addMenu(tr("&Export"));
		{ // Import menu
			QAction *act_export_gerber = export_menu->addAction(tr("Gerber"));
			QAction *act_export_dxf = export_menu->addAction(tr("DXF"));
			QAction *act_export_svg = export_menu->addAction(tr("SVG"));
			UNUSED(act_export_gerber);
			UNUSED(act_export_dxf);
			UNUSED(act_export_svg);
		}
		QMenu *import_menu = menu_file->addMenu(tr("&Import"));
		{ // Import menu
			QAction *act_import_gerber = import_menu->addAction(tr("Gerber"));
			connect(act_import_gerber, SIGNAL (triggered(bool)), this, SLOT (OpenImportGerberDialog()));
			QAction *act_import_gds = import_menu->addAction(tr("GDSII"));
			QAction *act_import_svg = import_menu->addAction(tr("SVG"));
			UNUSED(act_import_gds);
			UNUSED(act_import_svg);
		}
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
	}

	QMenu *menu_view = menubar->addMenu(tr("&View"));
	QMenu *menu_view_showhide = menu_view->addMenu(tr("Show/Hide"));
	QAction *menu_view_restore = menu_view->addAction(tr("&Restore"));
	UNUSED(menu_view_restore);

	setMenuBar(menubar);


	{
		QDockWidget *dock = new QDockWidget(tr("Parameters"), this);
		dock->setObjectName("dock_parameters");
		m_parameter_viewer = new ParameterViewer(dock,this);
		dock->setWidget(m_parameter_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::RightDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	{
		QDockWidget *dock = new QDockWidget(tr("Layers"), this);
		dock->setObjectName("dock_layers");
		m_layer_viewer = new LayerViewer(dock,this);
		dock->setWidget(m_layer_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	{
		QDockWidget *dock = new QDockWidget(tr("Libraries"), this);
		dock->setObjectName("dock_libraries");
		m_library_viewer = new LibraryViewer(dock,this);
		m_library_viewer->setModel(m_library_manager);
		m_library_viewer->hideColumn(1); //TODO remove once the searchable lib viewer is made with a proxy model (IF this is not there, the drag and drop is broken)
		dock->setWidget(m_library_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	m_document_viewer = new DocumentViewer(this,this);
	setCentralWidget(m_document_viewer);

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
	{ // UNDO
		QAction *undoAct;
		undoAct = new QAction(QIcon::fromTheme("edit-undo"), tr("Undo"), this);
		undoAct->setShortcuts(QKeySequence::Undo);
		undoAct->setStatusTip(tr("Undo"));
		connect(undoAct, SIGNAL(triggered()), this, SLOT(Undo()));
		toolbar->addAction(undoAct);
	}
	{ // REDO
		QAction *redoAct;
		redoAct = new QAction(QIcon::fromTheme("edit-redo"), tr("Redo"), this);
		redoAct->setShortcuts(QKeySequence::Redo);
		redoAct->setStatusTip(tr("Redo"));
		connect(redoAct, SIGNAL(triggered()), this, SLOT(Redo()));
		toolbar->addAction(redoAct);
	}

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

void MainWindow::OpenImportGerberDialog()
{
	if(m_import_gerber_dialog == NULL) {
		m_import_gerber_dialog = new GerberImportDialog(this);
		m_import_gerber_dialog->show();
	} else {
		m_import_gerber_dialog->raise();
	}
}

void MainWindow::CloseImportGerberDialog()
{
	if(m_import_gerber_dialog != NULL) {
		//m_import_gerber_dialog->deleteLater();
		m_import_gerber_dialog = NULL;
	}
}

void MainWindow::Undo()
{
	if(m_document_viewer->HasActiveDocument()) {
		m_document_viewer->GetActiveDocument()->GetDrawing()->HistoryUndo();
	}
	m_parameter_viewer->UpdateParameters(); //TODO FIgure out where this update should go
}

void MainWindow::Redo()
{
	if(m_document_viewer->HasActiveDocument()) {
		m_document_viewer->GetActiveDocument()->GetDrawing()->HistoryRedo();
	}
	m_parameter_viewer->UpdateParameters(); //TODO FIgure out where this update should go
}

