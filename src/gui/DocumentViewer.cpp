#include "Document.h"
#include "DocumentViewer.h"
#include "DrawingViewer.h"
#include "components/TabbedDocumentViewer.h"
#include "LayerManager.h"
#include "LibraryManager.h"
#include "Library.h"
#include "Drawing.h"
#include "Document.h"
#include "Icons.h"
#include "MainWindow.h"
#include <iostream>

DocumentViewer::DocumentViewer(QWidget *parent, MainWindow *main_window) : QWidget(parent)
{
	m_main_window = main_window;
	m_drawing_viewer  = NULL;
	m_active_document = NULL;

	m_drawing_viewer = new DrawingViewer(this);

	m_base = new QWidget(this);
	m_tabbar = new QTabBar(this);
	m_tabbar->setTabsClosable(true);
	m_tabbar->setMovable(true);
	m_tabbar->setExpanding(false);
	connect(m_tabbar,SIGNAL(currentChanged(int)),this,SLOT(OnActiveDocumentChange(int)));
	connect(m_tabbar,SIGNAL(tabCloseRequested(int)),this,SLOT(OnRemoveDocument(int)));
	connect(m_tabbar,SIGNAL(tabMoved(int, int)),this,SLOT(OnDocumentMove(int,int)));

	m_layout=new QVBoxLayout(m_base);
	m_layout->setMargin(2);
	m_layout->addWidget(m_drawing_viewer);

	updateLayout();
}

void DocumentViewer::OpenDrawing(Drawing *drawing) {

	if(IsDrawingOpen(drawing)){
		FocusDrawing(drawing);
	}
	else {
		m_main_window->GetLayerManager()->BeforeDocumentChange();
		TrackingPointer<Document> document(new Document(this,drawing));
		m_documents.emplace_back(std::move(document));

		QIcon icon;
		switch (drawing->GetType()) {
			case DRAWINGTYPE_SCHEMATIC:
				icon =  g_icon_librarymanager_schematic;
				break;
			case DRAWINGTYPE_SYMBOL:
				icon =  g_icon_librarymanager_symbol;
				break;
			case DRAWINGTYPE_LAYOUT:
				icon =  g_icon_librarymanager_layout;
				break;
		}

		addTab(icon,QString::fromStdString(StringRegistry::GetString(drawing->GetName())));
		m_main_window->GetLayerManager()->AfterDocumentChange();
	}
}

void DocumentViewer::CloseDrawing(Drawing *drawing)
{
	unsigned int pos = 0;
	bool found = false;
	for(unsigned int i = 0; i < m_documents.size(); ++i) {
		Drawing *d = m_documents.at(i).Get()->GetDrawing();
		if(d == drawing){
			found = true;
			break;
		}
		else {
			pos++;
		}
	}

	if(found){OnRemoveDocument(pos);}
}



void DocumentViewer::addTab(QIcon icon, QString name)
{
	int tab_index = m_tabbar->addTab(icon, name);
	m_tabbar->setCurrentIndex(tab_index);
	updateLayout();

}

void DocumentViewer::addTab(QString name)
{
	int tab_index = m_tabbar->addTab(name);
	m_tabbar->setCurrentIndex(tab_index);
	updateLayout();

}

bool DocumentViewer::IsDrawingOpen(Drawing *drawing)
{
	for(index_t i = 0; i < m_documents.size(); ++i)
	{
		if(m_documents[i]->GetDrawing() == drawing){
			return true;
		}
	}
	return false;
}

void DocumentViewer::FocusDrawing(Drawing *drawing)
{
	for(index_t i = 0; i < m_documents.size(); ++i)
	{
		if(m_documents[i]->GetDrawing() == drawing){
			m_tabbar->setCurrentIndex(i);
			break;
		}
	}
}

void DocumentViewer::OnRemoveDocument(int index)
{
	if(m_tabbar->currentIndex() == index){
		m_main_window->GetLayerManager()->BeforeDocumentChange();
		m_active_document = NULL;
		m_main_window->GetLayerManager()->AfterDocumentChange();
	}
	DeleteFromVector(m_documents,index);
	m_tabbar->removeTab(index);
	updateLayout();

}

void DocumentViewer::OnDocumentMove(int from, int to)
{
	MoveInVector(m_documents, from, to);
}


void DocumentViewer::OnActiveDocumentChange(int index)
{
	m_main_window->GetLayerManager()->BeforeDocumentChange();
	if(index == -1) {
		m_active_document = NULL;
	} else {
		m_active_document = m_documents[index].Get();
	}
	m_main_window->GetLayerManager()->AfterDocumentChange();
	m_main_window->GetParameterViewer()->UpdateParameters();
}

void DocumentViewer::paintEvent(QPaintEvent *event)
{
	QStylePainter p(this);
	QStyleOptionTabWidgetFrameV2 opt;
	initStyleOption(&opt);
	opt.rect = m_panelrect;
	p.drawPrimitive(QStyle::PE_FrameTabWidget, opt);
	updateLayout();
	UNUSED(event);
}

void DocumentViewer::initStyleOption(QStyleOptionTabWidgetFrame *option) const{
	option->initFrom(this);
	option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);

	QSize t(m_base->width(), 0);
	if (m_base->isVisibleTo(const_cast<DocumentViewer *>(this))){
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

void DocumentViewer::updateLayout(void){
	QStyleOptionTabWidgetFrameV2 opt;
	initStyleOption(&opt);

	m_panelrect = style()->subElementRect(QStyle::SE_TabWidgetTabPane, &opt, this);
	m_base->setGeometry(style()->subElementRect(QStyle::SE_TabWidgetTabContents, &opt, this));
	m_tabbar->setGeometry(style()->subElementRect(QStyle::SE_TabWidgetTabBar, &opt, this));
	m_tabbar->setMinimumWidth(this->size().width());

	update();
	updateGeometry();
}

void DocumentViewer::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	updateLayout();
}

bool DocumentViewer::event(QEvent *ev)
{
	if (ev->type() == QEvent::LayoutRequest){
		updateLayout();
	}
	return QWidget::event(ev);
}

void DocumentViewer::changeEvent(QEvent *ev)
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

