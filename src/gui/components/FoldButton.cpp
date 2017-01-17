#include "FoldButton.h"
#include <iostream>

FoldButton::FoldButton(QWidget *parent = 0) : QPushButton(parent)
{

	setMinimumWidth(20);
	setMaximumWidth(20);

	m_state = FoldButtonSTATE_DISABLED;



	connect(this, SIGNAL (released()),this, SLOT (onClick()));
}

FoldButton::~FoldButton()
{

}

void FoldButton::setButtonState(FoldButtonSTATE state)
{
	m_state = state;
	if(state == FoldButtonSTATE_DISABLED) {
			setEnabled(false);
	}
}

void FoldButton::onClick()
{
	toggle_state();
	repaint();
}

void FoldButton::toggle_state()
{
	switch (m_state) {
		case FoldButtonSTATE_FOLDED:
			m_state = FoldButtonSTATE_UNFOLDED;
			break;
		case FoldButtonSTATE_UNFOLDED:
			m_state = FoldButtonSTATE_FOLDED;
			break;
		case FoldButtonSTATE_DISABLED:
			break;
	}
}

void FoldButton::paintEvent(QPaintEvent *event)
{
	QStyleOptionButton opt;

	switch (m_state) {
		case FoldButtonSTATE_FOLDED:
			opt.state = QStyle::State_Item | QStyle::State_Children | QStyle::State_Active | QStyle::State_Enabled;
			break;
		case FoldButtonSTATE_UNFOLDED:
			opt.state = QStyle::State_Item | QStyle::State_Children | QStyle::State_Open | QStyle::State_Active | QStyle::State_Enabled;
			break;
		case FoldButtonSTATE_DISABLED:
			opt.state = QStyle::State_Item ;
			break;
	}

	if(m_hover == true){
		opt.state |= QStyle::State_MouseOver;
	}
	else {
		opt.state &= ~QStyle::State_MouseOver;
	}


	QPainter painter(this);
	opt.rect = rect();
	style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, &painter, this);
}

void FoldButton::enterEvent(QEvent *event)
{
	m_hover = true;
	QWidget::enterEvent(event);
}

void FoldButton::leaveEvent(QEvent *event)
{
	m_hover = false;
	QWidget::leaveEvent(event);
}
