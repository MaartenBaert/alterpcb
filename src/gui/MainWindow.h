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

#pragma once

#include "Basics.h"
#include "DocumentEditor.h"

#include <QtGui>

class LibraryManager;
class LayerManager;
class LibraryConfigDialog;
class LayerConfigDialog;

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	static const QString WINDOW_TITLE;
	LibraryManager *m_library_manager;
	LayerManager *m_layer_manager;
	LibraryConfigDialog *m_library_config_dialog;
	LayerConfigDialog *m_layer_config_dialog;


private:
	// member variables here

public:
	MainWindow(LibraryManager* library_manager, LayerManager* layer_manager);
	~MainWindow();

public slots:
	void OpenLibraryConfigDialog();
	void CloseLibraryConfigDialog();
	void OpenLayerConfigDialog();
	void CloseLayerConfigDialog();

};
