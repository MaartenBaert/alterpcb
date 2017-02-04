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

#include <QtGui>

class LayerManager;

class LayerWidget: public QWidget
{
	Q_OBJECT

private:
	LayerManager *m_layer_mananger;

	static const int LAYOUT_TOTAL_WIDTH = 200;
	static const int LAYOUT_TOTAL_SPACING = 20;

	static const int LAYOUT_METAL_HEIGHT = 20;
	const QColor LAYOUT_METAL_COLOR = QColor(128, 255, 255);
	static const int LAYOUT_DIE_HEIGHT = 20;
	const QColor LAYOUT_DIE_COLOR = QColor(128, 255, 255);

public:
	LayerWidget(QWidget *parent, LayerManager* layer_manager);

protected:
	virtual void paintEvent(QPaintEvent* event)	override;

};
