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

#include "Shape.h"
#include "VData.h"
#include "HashTable.h"

#include <vector>
#include <QtGui>

class MainWindow;

enum HOVER_REGION {
	HOVER_REGION_NONE,
	HOVER_REGION_FOLDBUTTON,
	HOVER_REGION_SELECTBUTTON,
	HOVER_REGION_DESELECTBUTTON,
	HOVER_REGION_OVERRIDEBUTTON,
	HOVER_REGION_LABEL,
};

//******************************************************************************************//
struct SubParameterEntry {
	index_t m_num_shapes;
	QWidget *m_widget;

	inline SubParameterEntry(index_t num_shapes,QWidget *widget) : m_num_shapes(num_shapes), m_widget(widget) {}
};

struct ParameterEntry {
	stringtag_t m_name;
	VData m_value;
	bool m_override;
	bool m_mergeable;
	bool m_expanded;
	std::vector<SubParameterEntry> m_subparameters;
	inline ParameterEntry(stringtag_t name, const VData &value, bool override, bool mergeable, bool folded)
		: m_name(name), m_value(value), m_override(override), m_mergeable(mergeable), m_expanded(folded) {}
	inline ParameterEntry(stringtag_t name, VData &&value, bool override, bool mergeable, bool folded)
		: m_name(name), m_value(std::move(value)), m_override(override), m_mergeable(mergeable), m_expanded(folded) {}
};

//******************************************************************************************//
struct ParameterHasher {
	inline bool Equal(const ParameterEntry &a, const ParameterEntry &b) const {
		return a.m_name == b.m_name;
	}
	inline bool Equal(const ParameterEntry &a, stringtag_t b) const {
		return a.m_name == b;
	}
	inline hash_t Hash(hash_t hash, const ParameterEntry &value) const {
		return MurmurHash::HashData(hash, value.m_name);
	}
	inline hash_t Hash(hash_t hash, stringtag_t value) const {
		return MurmurHash::HashData(hash, value);
	}
};

//******************************************************************************************//
class ParameterViewer : public QAbstractScrollArea {
	Q_OBJECT

private:
	MainWindow *m_mainwindow;
	std::vector<QWidget*> m_widgets;
	std::vector<Shape*> m_selected_shapes;
	HashTable<ParameterEntry, ParameterHasher> m_parameters;
	HOVER_REGION m_hover_region;
	bool m_button_pressed;
	index_t m_current_index;
	index_t m_current_subindex;

	static const int LAYOUT_FOLDBUTTONWIDTH = 20;
	static const int LAYOUT_SUBPARAMBUTTONWIDTH = 16;
	static const int LAYOUT_VSPACING = 4; // best keep even number
	static const int LAYOUT_HSPACING = 3;
	static const int LAYOUT_OVERRIDEBUTTONWIDTH = 16;
	int LAYOUT_LABELWIDTH = 70;

public:
	ParameterViewer(QWidget *parent, MainWindow *mainwindow);
	~ParameterViewer();

	index_t GetWidgetCount();
	QWidget* GetWidget(index_t index);

	void AddWidget(index_t index, QWidget* widget);
	void RemoveWidget(index_t index);
	void MoveWidget(index_t from, index_t to);

	void MakeVisible(QWidget* widget);

protected:
	virtual bool viewportEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void scrollContentsBy(int dx, int dy) override;
	virtual bool focusNextPrevChild(bool next) override;
	virtual void paintEvent(QPaintEvent* event)	override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void leaveEvent(QEvent * event) override;

private:
	void UpdateFocusChain();
	void UpdateRange();
	void UpdateLayout();

	void positionToIndex(const QPoint &pos);
	void ExpandParameter(index_t index);
	void UnexpandParameter(index_t index);
	HOVER_REGION getHoverRegion(const QPoint &pos);
	void changeHoverRegion(HOVER_REGION hover_region);
	void ensureWidgetVisible(QWidget *childWidget);

	void loadTestParam();
};




