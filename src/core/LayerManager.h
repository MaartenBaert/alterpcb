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

#include <vector>
#include "StringRegistry.h"

#include <QtGui>

enum LAYERTEXTURE{
	LAYERTEXTURE_SOLID,
};

struct LAYER{
	stringtag_t m_name;
	bool m_displayed;
	bool m_selectable;
	QColor m_color;
	LAYERTEXTURE m_texture;

	inline LAYER(stringtag_t name, bool displayed, bool selectable, QColor color, LAYERTEXTURE texture)
		: m_name(name), m_displayed(displayed), m_selectable(selectable), m_color(color), m_texture(texture) {}
};

class LayerManager : public QAbstractItemModel {
	Q_OBJECT

private:
	std::vector<LAYER*> m_layers;

public:
	LayerManager();

	QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	void activeLayerChanged(QModelIndex &index);

};

