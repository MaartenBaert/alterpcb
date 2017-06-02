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
class CloseButton;

class TearOutTabbar : public QWidget
{
	Q_OBJECT

public:
	enum ButtonPosition {
		LeftSide,
		RightSide
	};

	enum SelectionBehavior {
		SelectLeftTab,
		SelectRightTab,
		SelectPreviousTab
	};

	struct Tab {
		QString m_text;
		QIcon m_icon;
		CloseButton *m_button;
		ButtonPosition m_button_position;
		bool m_enabled;

		inline Tab(const QString &text,const QIcon &icon,CloseButton *button, const ButtonPosition &button_position):
			m_text(text), m_icon(icon), m_button(button), m_button_position(button_position), m_enabled(true){}

		 bool operator==(const Tab &other) const { return &other == this; }
	};

private:
	int m_currentIndex;
	QList<Tab> m_tablist;


public:
	TearOutTabbar(QWidget *parent);

	int addTab(const QString &text);
	int addTab(const QIcon &icon, const QString &text);
	int insertTab(int index, const QIcon&icon, const QString &text);

	void removeTab(int index);
	void moveTab(int from, int to);

	bool isTabEnabled(int index) const;
	void setTabEnabled(int index, bool);

	QString tabText(int index) const;
	void setTabText(int index, const QString &text);

	QColor tabTextColor(int index) const;
	void setTabTextColor(int index, const QColor &color);

	QIcon tabIcon(int index) const;
	void setTabIcon(int index, const QIcon &icon);

	int currentIndex() const;

	QRect tabRect(int index) const;
	int tabAt(const QPoint &pos) const;

signals:
	void currentChanged(int index);
	void tabCloseRequested(int index);
	void tabMoved(int from, int to);

public slots:
	void setCurrentIndex(int index);

private:
	inline bool validIndex(int index) const { return index >= 0 && index < m_tablist.count(); }
	void makeVisible(int index);
	void layoutTab(int index);

protected:
	void initStyleOption(QStyleOptionTab *option, int tabIndex) const;
	//	void dragEnterEvent(QDragEnterEvent *event) override;
	//	void dragLeaveEvent(QDragLeaveEvent *event) override;
	//	void dragMoveEvent(QDragMoveEvent *event) override;
	//	void dropEvent(QDropEvent *event) override;
	//	void mousePressEvent(QMouseEvent *event) override;
	//	void mouseReleaseEvent(QMouseEvent *event) override;
	//	void mouseMoveEvent(QMouseEvent *event) override;
};


class CloseButton : public QAbstractButton
{
	Q_OBJECT

public:
	CloseButton(QWidget *parent = 0);

	QSize sizeHint() const;
	inline QSize minimumSizeHint() const
		{ return sizeHint(); }
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void paintEvent(QPaintEvent *event);
};

