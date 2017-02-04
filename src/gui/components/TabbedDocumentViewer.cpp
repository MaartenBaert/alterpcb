#include "TabbedDocumentViewer.h"

TabbedDocumentViewer::TabbedDocumentViewer(QWidget *parent) : QWidget(parent)
{
	m_base = new QWidget(this);
	m_tabbar = new QTabBar(this);
	connect(m_tabbar,SIGNAL(currentChanged(int)),this,SLOT(tabbarClicked(int)));
	m_layout=new QVBoxLayout(m_base);
	m_layout->setMargin(2);
	updateLayout();
}

void TabbedDocumentViewer::setCentralWidget(QWidget *central)
{
	// Remove all widgets
	while(m_layout->count() > 1){
		m_layout->removeItem(m_layout->takeAt(0));
	}

	m_layout->addWidget(central);
	UNUSED(central);
}

void TabbedDocumentViewer::addDocument(QIcon icon, QString name, const QVariant &document_pointer)
{
	int tab_index = m_tabbar->addTab(icon, name);
	m_tabbar->setTabData(tab_index,document_pointer);
	updateLayout();

}

void TabbedDocumentViewer::addDocument(QString name, const QVariant &document_pointer)
{
	int tab_index = m_tabbar->addTab(name);
	m_tabbar->setTabData(tab_index,document_pointer);
	updateLayout();

}

void TabbedDocumentViewer::removeDocument(const QVariant &document_pointer)
{
	int tabindex = 0; // TODO
//	if(tabindex == -1){
//		return;
//	}
	m_tabbar->removeTab(tabindex);

	updateLayout();

}

void TabbedDocumentViewer::tabbarClicked(int index)
{
	emit tabClicked(m_tabbar->tabData(index));
}

void TabbedDocumentViewer::paintEvent(QPaintEvent *event)
{
	QStylePainter p(this);
	QStyleOptionTabWidgetFrameV2 opt;
	initStyleOption(&opt);
	opt.rect = m_panelrect;
	p.drawPrimitive(QStyle::PE_FrameTabWidget, opt);
	updateLayout();
	UNUSED(event);
}

void TabbedDocumentViewer::initStyleOption(QStyleOptionTabWidgetFrame *option) const{
	option->initFrom(this);
	option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);

	QSize t(m_base->width(), 0);
	if (m_base->isVisibleTo(const_cast<TabbedDocumentViewer *>(this))){
		t = m_tabbar->sizeHint();
	}
	option->tabBarSize = t;
	option->leftCornerWidgetSize = QSize(0, 0);
	option->rightCornerWidgetSize = QSize(0, 0);

	if (QStyleOptionTabWidgetFrameV2 *tabframe = qstyleoption_cast<QStyleOptionTabWidgetFrameV2*>(option)){
		QRect tbRect = m_tabbar->geometry();
		QRect selectedTabRect = m_tabbar->tabRect(m_tabbar->currentIndex());
		tabframe->tabBarRect = tbRect;
		selectedTabRect.moveTopLeft(selectedTabRect.topLeft() + tbRect.topLeft());
		tabframe->selectedTabRect = selectedTabRect;
	}
}

void TabbedDocumentViewer::updateLayout(void){
	QStyleOptionTabWidgetFrameV2 opt;
	initStyleOption(&opt);

	m_panelrect = style()->subElementRect(QStyle::SE_TabWidgetTabPane, &opt, this);
	m_base->setGeometry(style()->subElementRect(QStyle::SE_TabWidgetTabContents, &opt, this));
	m_tabbar->setGeometry(style()->subElementRect(QStyle::SE_TabWidgetTabBar, &opt, this));

	update();
	updateGeometry();
}

void TabbedDocumentViewer::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	updateLayout();
}

bool TabbedDocumentViewer::event(QEvent *ev)
{
	if (ev->type() == QEvent::LayoutRequest){
		updateLayout();
	}
	return QWidget::event(ev);
}

void TabbedDocumentViewer::changeEvent(QEvent *ev)
{
	if (ev->type() == QEvent::StyleChange
#ifdef Q_WS_MAC
			|| ev->type() == QEvent::MacSizeChange
#endif
			){
		updateLayout();
	}
	QWidget::changeEvent(ev);
}
