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
#include "CoreBasics.h"

#include <QtGui>

class LibraryViewer : public QTreeView {
	Q_OBJECT

public:
	LibraryViewer(QWidget *parent = 0);
	~LibraryViewer();

	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	QRect m_drop_indicator_rect;

	DropLocation getDropLocation(QRect &index_rect, QPoint pos);

private slots:
	void OnDoubleClick(const QModelIndex &index);
	void OnRightClick(const QPoint &point);

};
