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

#include "DrawingViewer.h"

const QString MainWindow::WINDOW_TITLE = "AlterPCB";

MainWindow::MainWindow(LibraryManager* library_manager) {

	setWindowTitle(WINDOW_TITLE);

	DrawingViewer *drawing_viewer = new DrawingViewer(this);

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
		menu_edit->addAction("Test");
		menu_edit->addAction("Test");
		menu_edit->addAction("Test");
	}

	QMenu *menu_view = menubar->addMenu(tr("&View"));
	QMenu *menu_view_showhide = menu_view->addMenu(tr("Show/Hide"));
	QAction *menu_view_restore = menu_view->addAction(tr("&Restore"));

	UNUSED(menu_view_restore);

	setMenuBar(menubar);

	QTreeView *parameter_viewer;
	{
		QDockWidget *dock = new QDockWidget(tr("Parameters"), this);
		dock->setObjectName("dock_parameters");
		parameter_viewer = new QTreeView(dock);
		parameter_viewer->setUniformRowHeights(true);
		dock->setWidget(parameter_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::RightDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	QTreeView *layer_viewer;
	{
		QDockWidget *dock = new QDockWidget(tr("Layers"), this);
		dock->setObjectName("dock_layers");
		layer_viewer = new QTreeView(dock);
		layer_viewer->setUniformRowHeights(true);
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
		library_viewer->setUniformRowHeights(true);
		library_viewer->setModel(library_manager);
		library_viewer->setHeaderHidden(true);
		library_viewer->setDragEnabled(true);
		library_viewer->setAcceptDrops(true);
		library_viewer->setDropIndicatorShown(true);
		library_viewer->setSelectionBehavior(QAbstractItemView::SelectRows);
		library_viewer->setDragDropMode(QAbstractItemView::InternalMove);
		dock->setWidget(library_viewer);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
		menu_view_showhide->addAction(dock->toggleViewAction());
	}

	setCentralWidget(drawing_viewer);

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
	// nothing
}
