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

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <QtGui>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QShortcut>
#include <QStatusBar>
#include <QStylePainter>
#include <QTableWidget>
#include <QTextBrowser>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_2_Core>

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
