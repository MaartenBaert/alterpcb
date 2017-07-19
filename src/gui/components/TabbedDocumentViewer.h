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
#include "Document.h"
#include "Qt.h"

#include <vector>

class TabbedDocumentViewer : public QWidget {
	Q_OBJECT

private:
	QTabBar *m_tabbar;
	QWidget *m_base;
	QRect m_panelrect;
	QVBoxLayout *m_layout;
	std::vector<Document*> m_documents;

public:
	explicit TabbedDocumentViewer(QWidget *parent = 0);
	void setCentralWidget(QWidget *central);
	void addDocument(QIcon icon, QString name, Document *ptr);
	void addDocument(QString name, Document *ptr);
	void removeDocument(Document *ptr);

private:
	void updateLayout();
	void initStyleOption(QStyleOptionTabWidgetFrame *option) const;

	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	bool event(QEvent *event);
	void changeEvent(QEvent *event);

signals:
	void tabClicked(Document *ptr);


private slots:
	void tabbarClicked(int index);
};
