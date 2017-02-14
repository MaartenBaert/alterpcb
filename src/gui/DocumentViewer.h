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
#include "TabbedDocumentViewer.h"
#include "Document.h"
#include "TrackingPointer.h"

#include <QWidget>
#include <QtGui>

class Drawing;
class MainWindow;
class DrawingViewer;

class DocumentViewer : public QWidget
{
	Q_OBJECT

private:
	std::vector<TrackingPointer<Document>> m_documents;
	MainWindow *m_main_window;
	DrawingViewer *m_drawing_viewer;
	Document *m_active_document;

	QTabBar *m_tabbar;
	QWidget *m_base;
	QRect m_panelrect;
	QVBoxLayout *m_layout;

public:
	DocumentViewer(QWidget *parent, MainWindow *main_window);

	void OpenDrawing(Drawing *drawing);

public:
	inline Document* GetActiveDocument() { return m_active_document; }

private:
	void addTab(QIcon icon, QString name);
	void addTab(QString name);
	bool IsDrawingOpen(Drawing *drawing);
	void FocusDrawing(Drawing *drawing);

	void updateLayout();
	void initStyleOption(QStyleOptionTabWidgetFrame *option) const;

	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent *e);
	bool event(QEvent *ev);
	void changeEvent(QEvent *ev);

public slots:
	void OnActiveDocumentChange(int index);
	void OnRemoveDocument(int index);
	void OnDocumentMove(int from, int to);
};

