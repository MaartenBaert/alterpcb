#include "TearOutTabbar.h"
#include <iostream>

TearOutTabbar::TearOutTabbar(QWidget *parent) : QWidget(parent){
	setAcceptDrops(false);

}

int TearOutTabbar::addTab(const QString &text)
{
	return insertTab(-1,QIcon(), text);
}

int TearOutTabbar::addTab(const QIcon& icon, const QString &text)
{
	return insertTab(-1, icon, text);
}

int TearOutTabbar::insertTab(int index, const QIcon& icon, const QString &text)
{
	QStyleOptionTabV3 opt;
	initStyleOption(&opt, index);
	ButtonPosition closeSide = (ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
	CloseButton *closeButton = new CloseButton(this);
	//connect(closeButton, SIGNAL(clicked()), this, SLOT(_q_closeTab()));

	if (validIndex(index)) {
		index = m_tablist.count();
		m_tablist.append(Tab(text,icon,closeButton,closeSide));
	} else {
		m_tablist.insert(index,Tab(text,icon,closeButton,closeSide));
	}

	return index;
}


void TearOutTabbar::removeTab(int index)
{
//	if (validIndex(index)) {

//		if (d->tabList[index].leftWidget) {
//			d->tabList[index].leftWidget->hide();
//			d->tabList[index].leftWidget->deleteLater();
//			d->tabList[index].leftWidget = 0;
//		}
//		if (d->tabList[index].rightWidget) {
//			d->tabList[index].rightWidget->hide();
//			d->tabList[index].rightWidget->deleteLater();
//			d->tabList[index].rightWidget = 0;
//		}

//		int newIndex = d->tabList[index].lastTab;
//		d->tabList.removeAt(index);
//		for (int i = 0; i < d->tabList.count(); ++i) {
//			if (d->tabList[i].lastTab == index)
//				d->tabList[i].lastTab = -1;
//			if (d->tabList[i].lastTab > index)
//				--d->tabList[i].lastTab;
//		}
//		if (index == d->currentIndex) {
//			// The current tab is going away, in order to make sure
//			// we emit that "current has changed", we need to reset this
//			// around.
//			d->currentIndex = -1;
//			if (d->tabList.size() > 0) {
//				switch(d->selectionBehaviorOnRemove) {
//					case SelectPreviousTab:
//						if (newIndex > index)
//							newIndex--;
//						if (d->validIndex(newIndex))
//							break;
//						// else fallthrough
//					case SelectRightTab:
//						newIndex = index;
//						if (newIndex >= d->tabList.size())
//							newIndex = d->tabList.size() - 1;
//						break;
//					case SelectLeftTab:
//						newIndex = index - 1;
//						if (newIndex < 0)
//							newIndex = 0;
//						break;
//					default:
//						break;
//				}

//				if (d->validIndex(newIndex)) {
//					// don't loose newIndex's old through setCurrentIndex
//					int bump = d->tabList[newIndex].lastTab;
//					setCurrentIndex(newIndex);
//					d->tabList[newIndex].lastTab = bump;
//				}
//			} else {
//				emit currentChanged(-1);
//			}
//		} else if (index < d->currentIndex) {
//			setCurrentIndex(d->currentIndex - 1);
//		}
//		d->refresh();
//		tabRemoved(index);
//	}
}



bool TearOutTabbar::isTabEnabled(int index) const
{
	return false;
}


void TearOutTabbar::setTabEnabled(int index, bool enabled)
{

}



QString TearOutTabbar::tabText(int index) const
{

	return QString();
}


void TearOutTabbar::setTabText(int index, const QString &text)
{

}


QColor TearOutTabbar::tabTextColor(int index) const
{

	return QColor();
}


void TearOutTabbar::setTabTextColor(int index, const QColor &color)
{

}


QIcon TearOutTabbar::tabIcon(int index) const
{

	return QIcon();
}


void TearOutTabbar::setTabIcon(int index, const QIcon & icon)
{

}

int TearOutTabbar::currentIndex() const
{
	return m_currentIndex;
}

void TearOutTabbar::setCurrentIndex(int index)
{
	int oldIndex = m_currentIndex;
	if (validIndex(index) && m_currentIndex != index) {
		m_currentIndex = index;
		update();
		makeVisible(index);
		layoutTab(oldIndex);
		layoutTab(index);
		emit currentChanged(index);
	}
}

QRect TearOutTabbar::tabRect(int index) const
{
//    Q_D(const QTabBar);
//    if (const QTabBarPrivate::Tab *tab = d->at(index)) {
//        if (d->layoutDirty)
//            const_cast<QTabBarPrivate*>(d)->layoutTabs();
//        QRect r = tab->rect;
//        if (verticalTabs(d->shape))
//            r.translate(0, -d->scrollOffset);
//        else
//            r.translate(-d->scrollOffset, 0);
//        if (!verticalTabs(d->shape))
//            r = QStyle::visualRect(layoutDirection(), rect(), r);
//        return r;
//    }
	return QRect();
}


int TearOutTabbar::tabAt(const QPoint &position) const
{
//	if (validIndex(currentIndex) && tabRect(currentIndex).contains(position)) {
//		return currentIndex;
//	}
//	const int max = m_tablist.size();
//	for (int i = 0; i < max; ++i) {
//		if (tabRect(i).contains(position)) {
//			return i;
//		}
//	}
	return -1;
}




void TearOutTabbar::initStyleOption(QStyleOptionTab *option, int tabIndex) const
{
//	Q_D(const QTabBar);
//	int totalTabs = d->tabList.size();

//	if (!option || (tabIndex < 0 || tabIndex >= totalTabs))
//		return;

//	const QTabBarPrivate::Tab &tab = d->tabList.at(tabIndex);
//	option->initFrom(this);
//	option->state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);
//	option->rect = tabRect(tabIndex);
//	bool isCurrent = tabIndex == d->currentIndex;
//	option->row = 0;
//	if (tabIndex == d->pressedIndex)
//		option->state |= QStyle::State_Sunken;
//	if (isCurrent)
//		option->state |= QStyle::State_Selected;
//	if (isCurrent && hasFocus())
//		option->state |= QStyle::State_HasFocus;
//	if (!tab.enabled)
//		option->state &= ~QStyle::State_Enabled;
//	if (isActiveWindow())
//		option->state |= QStyle::State_Active;
//	if (!d->dragInProgress && option->rect == d->hoverRect)
//		option->state |= QStyle::State_MouseOver;
//	option->shape = d->shape;
//	option->text = tab.text;

//	if (tab.textColor.isValid())
//		option->palette.setColor(foregroundRole(), tab.textColor);

//	option->icon = tab.icon;
//	if (QStyleOptionTabV2 *optionV2 = qstyleoption_cast<QStyleOptionTabV2 *>(option))
//		optionV2->iconSize = iconSize();  // Will get the default value then.

//	if (QStyleOptionTabV3 *optionV3 = qstyleoption_cast<QStyleOptionTabV3 *>(option)) {
//		optionV3->leftButtonSize = tab.leftWidget ? tab.leftWidget->size() : QSize();
//		optionV3->rightButtonSize = tab.rightWidget ? tab.rightWidget->size() : QSize();
//		optionV3->documentMode = d->documentMode;
//	}

//	if (tabIndex > 0 && tabIndex - 1 == d->currentIndex)
//		option->selectedPosition = QStyleOptionTab::PreviousIsSelected;
//	else if (tabIndex + 1 < totalTabs && tabIndex + 1 == d->currentIndex)
//		option->selectedPosition = QStyleOptionTab::NextIsSelected;
//	else
//		option->selectedPosition = QStyleOptionTab::NotAdjacent;

//	bool paintBeginning = (tabIndex == 0) || (d->dragInProgress && tabIndex == d->pressedIndex + 1);
//	bool paintEnd = (tabIndex == totalTabs - 1) || (d->dragInProgress && tabIndex == d->pressedIndex - 1);
//	if (paintBeginning) {
//		if (paintEnd)
//			option->position = QStyleOptionTab::OnlyOneTab;
//		else
//			option->position = QStyleOptionTab::Beginning;
//	} else if (paintEnd) {
//		option->position = QStyleOptionTab::End;
//	} else {
//		option->position = QStyleOptionTab::Middle;
//	}

//#ifndef QT_NO_TABWIDGET
//	if (const QTabWidget *tw = qobject_cast<const QTabWidget *>(parentWidget())) {
//		if (tw->cornerWidget(Qt::TopLeftCorner) || tw->cornerWidget(Qt::BottomLeftCorner))
//			option->cornerWidgets |= QStyleOptionTab::LeftCornerWidget;
//		if (tw->cornerWidget(Qt::TopRightCorner) || tw->cornerWidget(Qt::BottomRightCorner))
//			option->cornerWidgets |= QStyleOptionTab::RightCornerWidget;
//	}
//#endif

//	QRect textRect = style()->subElementRect(QStyle::SE_TabBarTabText, option, this);
//	option->text = fontMetrics().elidedText(option->text, d->elideMode, textRect.width(),
//						Qt::TextShowMnemonic);
}




void TearOutTabbar::makeVisible(int index)
{
//    Q_Q(QTabBar);
//    if (!validIndex(index) || leftB->isHidden())
//        return;

//    const QRect tabRect = tabList.at(index).rect;
//    const int oldScrollOffset = scrollOffset;
//    const bool horiz = !verticalTabs(shape);
//    const int available = (horiz ? q->width() : q->height()) - extraWidth();
//    const int start = horiz ? tabRect.left() : tabRect.top();
//    const int end = horiz ? tabRect.right() : tabRect.bottom();
//    if (start < scrollOffset) // too far left
//        scrollOffset = start - (index ? 8 : 0);
//    else if (end > scrollOffset + available) // too far right
//        scrollOffset = end - available + 1;

//    leftB->setEnabled(scrollOffset > 0);
//    const int last = horiz ? tabList.last().rect.right() : tabList.last().rect.bottom();
//    rightB->setEnabled(last - scrollOffset >= available);
//    if (oldScrollOffset != scrollOffset) {
//        q->update();
//        layoutWidgets();
//    }
}

void TearOutTabbar::layoutTab(int index)
{
//    Q_Q(QTabBar);
//    Q_ASSERT(index >= 0);

//    Tab &tab = tabList[index];
//    bool vertical = verticalTabs(shape);
//    if (!(tab.leftWidget || tab.rightWidget))
//        return;

//    QStyleOptionTabV3 opt;
//    q->initStyleOption(&opt, index);
//    if (tab.leftWidget) {
//        QRect rect = q->style()->subElementRect(QStyle::SE_TabBarTabLeftButton, &opt, q);
//        QPoint p = rect.topLeft();
//        if ((index == pressedIndex) || paintWithOffsets) {
//            if (vertical)
//                p.setY(p.y() + tabList[index].dragOffset);
//            else
//                p.setX(p.x() + tabList[index].dragOffset);
//        }
//        tab.leftWidget->move(p);
//    }
//    if (tab.rightWidget) {
//        QRect rect = q->style()->subElementRect(QStyle::SE_TabBarTabRightButton, &opt, q);
//        QPoint p = rect.topLeft();
//        if ((index == pressedIndex) || paintWithOffsets) {
//            if (vertical)
//                p.setY(p.y() + tab.dragOffset);
//            else
//                p.setX(p.x() + tab.dragOffset);
//        }
//        tab.rightWidget->move(p);
//    }
}
//======================================================================================================================================
// CLOSE BUTTON

CloseButton::CloseButton(QWidget *parent)
	: QAbstractButton(parent)
{
	setFocusPolicy(Qt::NoFocus);
	setCursor(Qt::ArrowCursor);
	setToolTip(tr("Close Tab"));
	resize(sizeHint());
}

QSize CloseButton::sizeHint() const
{
	ensurePolished();
	int width = style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, this);
	int height = style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, 0, this);
	return QSize(width, height);
}

void CloseButton::enterEvent(QEvent *event)
{
	if (isEnabled())
		update();
	QAbstractButton::enterEvent(event);
}

void CloseButton::leaveEvent(QEvent *event)
{
	if (isEnabled())
		update();
	QAbstractButton::leaveEvent(event);
}

void CloseButton::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	QStyleOption opt;
	opt.init(this);
	opt.state |= QStyle::State_AutoRaise;
	if (isEnabled() && underMouse() && !isChecked() && !isDown())
		opt.state |= QStyle::State_Raised;
	if (isChecked())
		opt.state |= QStyle::State_On;
	if (isDown())
		opt.state |= QStyle::State_Sunken;

	if (const TearOutTabbar *tb = qobject_cast<const TearOutTabbar *>(parent())) {
		int index = tb->currentIndex();
		TearOutTabbar::ButtonPosition position = (TearOutTabbar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, tb);
		if (tb->tabButton(index, position) == this)
			opt.state |= QStyle::State_Selected;
	}

	style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &p, this);
}
