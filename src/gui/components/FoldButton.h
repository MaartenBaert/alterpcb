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

enum FoldButtonSTATE {
	FoldButtonSTATE_FOLDED,
	FoldButtonSTATE_UNFOLDED,
	FoldButtonSTATE_DISABLED
};

class FoldButton : public QPushButton {
	Q_OBJECT

private:
	FoldButtonSTATE m_state;
	bool m_hover = false;

public:
	FoldButton(QWidget *parent);
	~FoldButton();

	void setButtonState(FoldButtonSTATE state);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private slots:
	void onClick();

private:
	void toggle_state();
	void paintEvent(QPaintEvent *event);


};
