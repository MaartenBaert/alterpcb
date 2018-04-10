#include "ParameterViewer.h"

#include "Drawing.h"
#include "Icons.h"
#include "Json.h"
#include "MainWindow.h"
#include "ShapeDefinition.h"
#include "ShapeInstance.h"
#include "ShapePrototype.h"
#include "StringRegistry.h"

#include <sstream>
#include <iostream>
#include <map>


ParameterViewer::ParameterViewer(QWidget *parent, MainWindow *mainwindow) : QAbstractScrollArea(parent) {
	m_mainwindow = mainwindow;
	m_hover_region = HOVER_REGION_NONE;
	m_button_pressed = false;
	m_current_index = INDEX_NONE;
	m_current_subindex = INDEX_NONE;

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // buggy with GTK style, maximumViewportSize() doesn't take the border into account :(
	verticalScrollBar()->setSingleStep(10);
	setFocusPolicy(Qt::NoFocus);

	viewport()->setBackgroundRole(QPalette::Window);
	viewport()->setAutoFillBackground(true);
	viewport()->setMouseTracking(true);

	UpdateRange();

}

ParameterViewer::~ParameterViewer() {}


static QSize GetWidgetSize(QWidget* widget)
{
	QSize size = widget->sizeHint();
	if(!size.isValid())
		size = QSize(0, 0);
	QSize minimumsize = widget->minimumSize();
	if(!minimumsize.isValid())
		minimumsize = widget->minimumSizeHint();
	if(minimumsize.isValid())
		size.expandedTo(minimumsize);
	QSize maximumsize = widget->maximumSize();
	if(maximumsize.isValid())
		size.boundedTo(maximumsize);
	return size;
}

index_t ParameterViewer::GetWidgetCount()
{
	return m_parameters.GetSize();
}

QWidget* ParameterViewer::GetWidget(index_t index)
{
	assert(index < m_parameters.GetSize());
	return m_parameters[index].m_widget;
}

void ParameterViewer::UpdateParameters()
{
	HashTable<ParameterEntry, ParameterHasher> new_parameters;

	if(m_mainwindow->GetDocumentViewer()->HasActiveDocument()) {
		const std::vector<Cow<ShapeInstance>>& shapes = m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->GetShapes();


		// make new m_parameter
		// copy existing widgets + expanded info
		for(index_t i = 0 ; i < shapes.size(); ++i) {
			const ShapeInstance &shapeinstance = shapes[i].Ref();

			if(shapeinstance.IsSelected()) {
				const ShapePrototype &shapeprototype = shapeinstance.GetShapePrototype().Ref();

				ShapeDefinition *shape_definition = NULL; //TODO// get this from LibraryManager
				const EffectiveParameters &params = shapeprototype.GetEffectiveParameters(shape_definition);

				for(index_t j = 0 ; j < params.size(); ++j) {
					// does the parameter already exist in new_parameter
					//		if Yes is it mergable or not
					//			if No set bools + change widget text
					// if No does the parameter already exist in m_parameters
					//		if No make widget
					//		if Yes copy widget pointer + expanded


					std::pair<index_t, bool> added = new_parameters.TryEmplaceBack(
								params[j].GetName(), params[j].GetName(), params[j].GetValue(), params[j].IsOverride(),
								true, false, nullptr);

					if(!added.second) { // parameter exists
						if(new_parameters[added.first].m_value != params[j].GetValue()) { // not mergable
							new_parameters[added.first].m_mergeable = false;
							new_parameters[added.first].m_value = VData("...");
							static_cast<QLineEdit*>(new_parameters[added.first].m_widget)->setText("...");
						}
					}
					else {
						index_t index = m_parameters.Find(params[j].GetName());
						if(index == INDEX_NONE) { // Param doesnt not exist in m_paramters
							QLineEdit *valuebox = new QLineEdit(viewport());
							connect(valuebox,SIGNAL(editingFinished()),this,SLOT(OnParameterChange()));
							std::string str;
							Json::ToString(params[j].GetValue(),str);
							valuebox->setText(QString::fromStdString(str));
							valuebox->setAutoFillBackground(true);
							valuebox->setBackgroundRole(QPalette::Window);
							if(valuebox->focusPolicy() == Qt::NoFocus)
								valuebox->setFocusPolicy(Qt::ClickFocus);
							valuebox->show();
							new_parameters[added.first].m_widget = valuebox;
						}
						else { // Param exits in m_paramters copy widget pointer
							new_parameters[added.first].m_widget = m_parameters[index].m_widget;
							std::string str;
							Json::ToString(params[j].GetValue(),str);
							static_cast<QLineEdit*>(new_parameters[added.first].m_widget)->setText(QString::fromStdString(str));
							new_parameters[added.first].m_expanded = m_parameters[index].m_expanded;
							new_parameters[added.first].m_mergeable = true;
							m_parameters[index].m_widget = NULL;
						}
					}
				}
			}
		}
	}

	// remove unused widgets
	for(index_t i = 0 ; i < m_parameters.GetSize(); ++i) {
		if(m_parameters[i].m_widget != NULL) {
			m_parameters[i].m_widget->deleteLater();
		}
		for(index_t j = 0 ; j < m_parameters[i].m_subparameters.size(); ++j) {
			m_parameters[i].m_subparameters[j].m_widget->deleteLater();
		}
	}




	// copy new parameters to m_parameter
	m_parameters = std::move(new_parameters);

	// expand the needed parameters
	for(index_t i = 0; i < m_parameters.GetSize(); ++i)
	{
		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			ExpandParameter(i);
		}
	}

	UpdateFocusChain();
	UpdateRange();
	UpdateLayout();

}

ParameterNameValuePair ParameterViewer::GetParameterNameValue(QWidget *widget)
{
	stringtag_t param_name = STRINGTAG_NONE;
	VData param_value;
	for(index_t i = 0; i < m_parameters.GetSize(); ++i)
	{
		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			for(index_t j = 0; j < m_parameters[i].m_subparameters.size(); ++j){
				if(m_parameters[i].m_subparameters[j].m_widget == widget){
					param_name = m_parameters[i].m_name;
					param_value = m_parameters[i].m_subparameters[j].m_value;
				}

				if(param_name != STRINGTAG_NONE ){
					break;
				}
			}

		}

		if(m_parameters[i].m_widget == widget){
			param_name = m_parameters[i].m_name;
			param_value = m_parameters[i].m_value;
		}

		if(param_name != STRINGTAG_NONE ){
			break;
		}
	}

	return ParameterNameValuePair(param_name,param_value);
}

ParameterNameValuePair ParameterViewer::GetParameterNameValue()
{
	ParameterNameValuePair param_namevalue;
	if(m_current_subindex == INDEX_NONE) {
		param_namevalue = ParameterNameValuePair(m_parameters[m_current_index].m_name,m_parameters[m_current_index].m_value);
	}
	else {
		param_namevalue = ParameterNameValuePair(m_parameters[m_current_index].m_name,m_parameters[m_current_index].m_subparameters[m_current_subindex].m_value);
	}

	return param_namevalue;
}

bool ParameterViewer::viewportEvent(QEvent* event)
{
	switch(event->type()) {
		case QEvent::LayoutRequest: {
			UpdateRange();
			UpdateLayout();
			event->accept();
			return true;
		}
		default: return QAbstractScrollArea::viewportEvent(event);
	}
}

void ParameterViewer::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	UpdateRange();
	UpdateLayout();
}

void ParameterViewer::scrollContentsBy(int dx, int dy)
{
	if(dx == 0 && dy == 0)
		return;
	UpdateLayout();
}

bool ParameterViewer::focusNextPrevChild(bool next) {
	if (QWidget::focusNextPrevChild(next)) {
		if (QWidget *fw = focusWidget())
			ensureWidgetVisible(fw);
		return true;
	}
	return false;
}

void ParameterViewer::paintEvent(QPaintEvent *event)
{
	UNUSED(event);
	QPainter painter(viewport());

	int y = -verticalScrollBar()->value();
	y = y + LAYOUT_VSPACING;

	for(index_t i = 0; i < m_parameters.GetSize(); ++i)
	{
		QSize size = GetWidgetSize(m_parameters[i].m_widget);

		// INDEX FOLDBUTTON
		{
			QStyleOptionButton opt_foldbutton;
			opt_foldbutton.state = QStyle::State_Item | QStyle::State_Active | QStyle::State_Enabled;

			// FOLDBUTTON EXPANDED/UNEXPANDED STATE
			if(m_parameters[i].m_expanded && !m_parameters[i].m_mergeable){
				opt_foldbutton.state |= QStyle::State_Children | QStyle::State_Open;
			}
			else if(!m_parameters[i].m_expanded && !m_parameters[i].m_mergeable){
				opt_foldbutton.state |= QStyle::State_Children;
			}

			if(i < m_parameters.GetSize()-1){
				opt_foldbutton.state |= QStyle::State_Sibling;
			}

			// FOLDBUTTON MOUSE OVER
			if(m_current_index == i && m_current_subindex == INDEX_NONE && (m_hover_region == HOVER_REGION_FOLDBUTTON || m_hover_region == HOVER_REGION_LABEL)){
				opt_foldbutton.state |= QStyle::State_MouseOver;
			}
			else {
				opt_foldbutton.state &= ~QStyle::State_MouseOver;
			}

			opt_foldbutton.rect = QRect(0,y-LAYOUT_VSPACING/2,LAYOUT_FOLDBUTTONWIDTH,size.height()+LAYOUT_VSPACING);
			style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt_foldbutton, &painter, viewport());
		}

		// INDEX LABEL
		{
			painter.drawText(QRect(LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING,y,LAYOUT_LABELWIDTH-LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING,size.height()),Qt::AlignLeft|Qt::AlignVCenter|Qt::TextSingleLine,QString::fromStdString(StringRegistry::GetString(m_parameters[i].m_name)));
		}

		// INDEX OVERRIDE BUTTON
		{
			// ON STATE
			if(m_parameters[i].m_override){
				// PRESSED
				if(m_button_pressed && m_hover_region == HOVER_REGION_OVERRIDEBUTTON && m_current_index == i &&  m_current_subindex == INDEX_NONE){
					g_icon_parameterviewer_override_onpressed.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
				}
				// MOUSE OVER
				else if(m_hover_region == HOVER_REGION_OVERRIDEBUTTON && m_current_index == i &&  m_current_subindex == INDEX_NONE){
					g_icon_parameterviewer_override_onmouseover.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
				}
				// NORMAL
				else{
					g_icon_parameterviewer_override_onnormal.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
				}
			}
			// OFF STATE
			else{
				// PRESSED
				if(m_button_pressed && m_hover_region == HOVER_REGION_OVERRIDEBUTTON && m_current_index == i &&  m_current_subindex == INDEX_NONE){
					g_icon_parameterviewer_override_offpressed.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
				}
				// MOUSE OVER
				else if(m_hover_region == HOVER_REGION_OVERRIDEBUTTON && m_current_index == i &&  m_current_subindex == INDEX_NONE){
					g_icon_parameterviewer_override_offmouseover.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
				}
				// NORMAL
				else{
					g_icon_parameterviewer_override_offnormal.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
				}
			}
		}



		y += size.height()+LAYOUT_VSPACING;


		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			for(index_t j = 0; j < m_parameters[i].m_subparameters.size(); ++j){

				// SUBINDEX FOLDBUTTON
				if(i < m_parameters.GetSize()-1){
					QStyleOptionButton opt_foldbutton;
					opt_foldbutton.state = QStyle::State_Active | QStyle::State_Enabled | QStyle::State_Sibling;
					opt_foldbutton.rect = QRect(0,y-LAYOUT_VSPACING/2,LAYOUT_FOLDBUTTONWIDTH,size.height()+LAYOUT_VSPACING);
					style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt_foldbutton, &painter, viewport());
				}

				// SUBINDEX SELECTBUTTON
				{

					// PRESSED
					if(m_button_pressed && m_hover_region == HOVER_REGION_SELECTBUTTON && m_current_index == i &&  m_current_subindex == j){
						g_icon_parameterviewer_select_pressed.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING,y,LAYOUT_SUBPARAMBUTTONWIDTH,size.height());
					}
					// MOUSE OVER
					else if(m_hover_region == HOVER_REGION_SELECTBUTTON && m_current_index == i && m_current_subindex == j){
						g_icon_parameterviewer_select_mouseover.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING,y,LAYOUT_SUBPARAMBUTTONWIDTH,size.height());
					}
					// NORMAL
					else{
						g_icon_parameterviewer_select_normal.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING,y,LAYOUT_SUBPARAMBUTTONWIDTH,size.height());
					}
				}

				// SUBINDEX DESELECTBUTTON
				{

					// PRESSED
					if(m_button_pressed && m_hover_region == HOVER_REGION_DESELECTBUTTON && m_current_index == i &&  m_current_subindex == j){
						g_icon_parameterviewer_deselect_pressed.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_SUBPARAMBUTTONWIDTH,y,LAYOUT_SUBPARAMBUTTONWIDTH,size.height());
					}
					// MOUSE OVER
					else if(m_hover_region == HOVER_REGION_DESELECTBUTTON && m_current_index == i && m_current_subindex == j){
						g_icon_parameterviewer_deselect_mouseover.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_SUBPARAMBUTTONWIDTH,y,LAYOUT_SUBPARAMBUTTONWIDTH,size.height());
					}
					// NORMAL
					else{
						g_icon_parameterviewer_deselect_normal.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_SUBPARAMBUTTONWIDTH,y,LAYOUT_SUBPARAMBUTTONWIDTH,size.height());
					}
				}

				// SUBINDEX LABEL
				{
					painter.drawText(QRect(LAYOUT_FOLDBUTTONWIDTH+3*LAYOUT_HSPACING+2*LAYOUT_SUBPARAMBUTTONWIDTH,y,LAYOUT_LABELWIDTH-LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING,size.height()),Qt::AlignLeft|Qt::AlignVCenter|Qt::TextSingleLine,QString::fromStdString("(" + std::to_string(m_parameters[i].m_subparameters[j].m_num_shapes) + ")"));
				}

				// SUBINDEX OVERRIDE BUTTON
				{
					// ON STATE
					if(m_parameters[i].m_override){
						// PRESSED
						if(m_button_pressed && m_hover_region == HOVER_REGION_OVERRIDEBUTTON &&  m_current_index == i &&  m_current_subindex == j){
							g_icon_parameterviewer_override_onpressed.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
						}
						// MOUSE OVER
						else if(m_hover_region == HOVER_REGION_OVERRIDEBUTTON &&  m_current_index == i &&  m_current_subindex == j){
							g_icon_parameterviewer_override_onmouseover.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
						}
						// NORMAL
						else{
							g_icon_parameterviewer_override_onnormal.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
						}
					}
					// OFF STATE
					else{
						// PRESSED
						if(m_button_pressed && m_hover_region == HOVER_REGION_OVERRIDEBUTTON &&  m_current_index == i &&  m_current_subindex == j){
							g_icon_parameterviewer_override_offpressed.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
						}
						// MOUSE OVER
						else if(m_hover_region == HOVER_REGION_OVERRIDEBUTTON &&  m_current_index == i &&  m_current_subindex == j){
							g_icon_parameterviewer_override_offmouseover.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
						}
						// NORMAL
						else{
							g_icon_parameterviewer_override_offnormal.paint(&painter,LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH,y+(size.height()-LAYOUT_OVERRIDEBUTTONWIDTH)/2,LAYOUT_OVERRIDEBUTTONWIDTH,LAYOUT_OVERRIDEBUTTONWIDTH);
						}
					}
				}

				y += size.height()+LAYOUT_VSPACING;
			}
		}
	}

}

void ParameterViewer::mousePressEvent(QMouseEvent *event)
{
	UNUSED(event);
	switch (m_hover_region) {
		case HOVER_REGION_FOLDBUTTON:{
			if(!m_parameters[m_current_index].m_mergeable){
				if(m_parameters[m_current_index].m_expanded){
					UnexpandParameter(m_current_index);
				}
				else{
					ExpandParameter(m_current_index);
				}
				UpdateLayout();
				UpdateFocusChain();
				UpdateRange();
			}
			m_button_pressed = true;
			break;
		}
		case HOVER_REGION_SELECTBUTTON:{
			std::cerr << "SELECT BUTTON PRESSED : index = " << m_current_index << " subindex = " << m_current_subindex << std::endl;
			SelectShapes();
			m_button_pressed = true;
			break;
		}
		case HOVER_REGION_DESELECTBUTTON:{
			std::cerr << "DESELECT BUTTON PRESSED : index = " << m_current_index << " subindex = " << m_current_subindex << std::endl;
			DeselectShapes();
			m_button_pressed = true;
			break;
		}
		case HOVER_REGION_OVERRIDEBUTTON:{
			std::cerr << "OVERRIDE BUTTON PRESSED : index = " << m_current_index << " subindex = " << m_current_subindex << std::endl;
			OverrideShapes();
			m_button_pressed = true;
			break;
		}
		case HOVER_REGION_NONE:{
			break;
		}
		case HOVER_REGION_LABEL:{
			break;
		}
	}

	if(m_button_pressed){
		viewport()->update();
	}
}

void ParameterViewer::mouseReleaseEvent(QMouseEvent *event)
{
	UNUSED(event);
	if(m_button_pressed){
		m_button_pressed = false;
		viewport()->update();
	}
}


void ParameterViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
	UNUSED(event);
	if(m_hover_region == HOVER_REGION_LABEL){
		if(m_parameters[m_current_index].m_expanded){
			UnexpandParameter(m_current_index);
		}
		else{
			ExpandParameter(m_current_index);
		}
	}
	UpdateLayout();
	UpdateFocusChain();
	UpdateRange();

}

void ParameterViewer::mouseMoveEvent(QMouseEvent *event)
{
	changeHoverRegion(getHoverRegion(event->pos()));
}

void ParameterViewer::leaveEvent(QEvent *event)
{
	UNUSED(event);
	m_current_index = INDEX_NONE;
	m_current_subindex = INDEX_NONE;
	m_hover_region = HOVER_REGION_NONE;
	viewport()->update();
}

void ParameterViewer::UpdateFocusChain()
{
	QWidget *current_widget;
	QWidget *next_widget;

	for(index_t i = 0; i < m_parameters.GetSize(); ++i) {

		current_widget = m_parameters[i].m_widget;
		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			next_widget = m_parameters[i].m_subparameters[0].m_widget;
			setTabOrder(current_widget,next_widget);

			for(index_t j = 1; j < m_parameters[i].m_subparameters.size(); ++j){
				current_widget = next_widget;
				next_widget = m_parameters[i].m_subparameters[j].m_widget;
				setTabOrder(current_widget,next_widget);
			}
			current_widget = m_parameters[i].m_subparameters.back().m_widget;
		}
		else{
			if(i != m_parameters.GetSize()-1){
				next_widget = m_parameters[i+1].m_widget;
				setTabOrder(current_widget,next_widget);
			}
		}
	}
}

void ParameterViewer::UpdateRange() {
	int height = LAYOUT_VSPACING;
	for(index_t i = 0; i < m_parameters.GetSize(); ++i) {
		QSize size = GetWidgetSize(m_parameters[i].m_widget);
		height += size.height() + LAYOUT_VSPACING;

		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			for(index_t j = 0; j < m_parameters[i].m_subparameters.size(); ++j){
				QSize size = GetWidgetSize(m_parameters[i].m_subparameters[j].m_widget);
				height += size.height()+LAYOUT_VSPACING;
			}
		}

	}
	verticalScrollBar()->setPageStep(viewport()->height());
	verticalScrollBar()->setRange(0, std::max(0, height - viewport()->height()));
}

void ParameterViewer::UpdateLayout() {
	int y = -verticalScrollBar()->value();

	y = y + LAYOUT_VSPACING;
	for(index_t i = 0; i < m_parameters.GetSize(); ++i)
	{
		QSize size = GetWidgetSize(m_parameters[i].m_widget);

		m_parameters[i].m_widget->setGeometry(LAYOUT_LABELWIDTH+LAYOUT_FOLDBUTTONWIDTH+4*LAYOUT_HSPACING+LAYOUT_OVERRIDEBUTTONWIDTH, y, viewport()->width()-LAYOUT_LABELWIDTH-LAYOUT_FOLDBUTTONWIDTH-5*LAYOUT_HSPACING-LAYOUT_OVERRIDEBUTTONWIDTH, size.height());
		m_parameters[i].m_widget->setContentsMargins(0,0,0,0);
		if(!m_parameters[i].m_override) {
			static_cast<QLineEdit*>(m_parameters[i].m_widget)->setEnabled(false);
		}

		y += size.height()+LAYOUT_VSPACING;

		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			for(index_t j = 0; j < m_parameters[i].m_subparameters.size(); ++j){
				QSize size = GetWidgetSize(m_parameters[i].m_subparameters[j].m_widget);

				m_parameters[i].m_subparameters[j].m_widget->setGeometry(LAYOUT_LABELWIDTH+LAYOUT_FOLDBUTTONWIDTH+4*LAYOUT_HSPACING+LAYOUT_OVERRIDEBUTTONWIDTH, y, viewport()->width()-LAYOUT_LABELWIDTH-LAYOUT_FOLDBUTTONWIDTH-5*LAYOUT_HSPACING-LAYOUT_OVERRIDEBUTTONWIDTH, size.height());
				m_parameters[i].m_subparameters[j].m_widget->setContentsMargins(0,0,0,0);
				y += size.height()+LAYOUT_VSPACING;
			}
		}
	}

	viewport()->update();
}

void ParameterViewer::SelectShapes() {
	ParameterNameValuePair param_namevalue = GetParameterNameValue();

	const std::vector<Cow<ShapeInstance>>& shapes = m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->GetShapes();
	std::vector<Cow<ShapeInstance>> new_shapes;

	for(index_t i = 0 ; i < shapes.size(); ++i) {
		const ShapeInstance &shapeinstance = shapes[i].Ref();
		bool param_found = false;

		if(shapeinstance.IsSelected()) {
			const ShapePrototype &shapeprototype = shapeinstance.GetShapePrototype().Ref();

			ShapeDefinition *shape_definition = NULL; //TODO// get this from LibraryManager
			const EffectiveParameters &params = shapeprototype.GetEffectiveParameters(shape_definition);

			for(index_t j = 0 ; j < params.size(); ++j) {
				if(params[j].GetName() == param_namevalue.m_name && (params[j].GetValue() == param_namevalue.m_value || param_namevalue.m_value == VData("..."))) {
					param_found = true;
					new_shapes.emplace_back(shapes[i]);
					break;
				}
			}
		}
		if(!param_found) { // if parameter changed is not in params --> copy shape
			const Cow<ShapePrototype> &same_shapeprototype = shapeinstance.GetShapePrototype();
			Cow<ShapeInstance> new_shapeinstance(std::make_shared<ShapeInstance>(std::move(same_shapeprototype),false)); //TODO copy shape transform
			new_shapes.emplace_back(std::move(new_shapeinstance));
		}

	}
	m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->HistoryPush(std::move(new_shapes),false);
	UpdateParameters(); //TODO FIgure out where this update should go
}

void ParameterViewer::DeselectShapes()
{
	ParameterNameValuePair param_namevalue = GetParameterNameValue();

	const std::vector<Cow<ShapeInstance>>& shapes = m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->GetShapes();
	std::vector<Cow<ShapeInstance>> new_shapes;

	for(index_t i = 0 ; i < shapes.size(); ++i) {
		const ShapeInstance &shapeinstance = shapes[i].Ref();
		bool param_found = false;

		if(shapeinstance.IsSelected()) {
			const ShapePrototype &shapeprototype = shapeinstance.GetShapePrototype().Ref();

			ShapeDefinition *shape_definition = NULL; //TODO// get this from LibraryManager
			const EffectiveParameters &params = shapeprototype.GetEffectiveParameters(shape_definition);

			for(index_t j = 0 ; j < params.size(); ++j) {
				if(params[j].GetName() == param_namevalue.m_name && (params[j].GetValue() == param_namevalue.m_value || param_namevalue.m_value == VData("..."))) {
					param_found = true;
					const Cow<ShapePrototype> &same_shapeprototype = shapeinstance.GetShapePrototype();
					Cow<ShapeInstance> new_shapeinstance(std::make_shared<ShapeInstance>(std::move(same_shapeprototype),false)); //TODO copy shape transform
					new_shapes.emplace_back(std::move(new_shapeinstance));
					break;
				}
			}
		}
		if(!param_found) { // if parameter changed is in params --> copy shape
			new_shapes.emplace_back(shapes[i]);
		}

	}
	m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->HistoryPush(std::move(new_shapes),false);
	UpdateParameters(); //TODO FIgure out where this update should go
}

void ParameterViewer::OverrideShapes()
{

}

void ParameterViewer::positionToIndex(const QPoint &pos)
{
	m_current_index = INDEX_NONE;
	m_current_subindex = INDEX_NONE;
	int y = -verticalScrollBar()->value();

	y = y + LAYOUT_VSPACING/2;
	index_t i = 0;
	while(i < m_parameters.GetSize() && m_current_index == INDEX_NONE)
	{
		QSize size = GetWidgetSize(m_parameters[i].m_widget);
		y += size.height()+LAYOUT_VSPACING;

		if(pos.y() < y){
			m_current_index = i;
			break;
		}

		if(!m_parameters[i].m_mergeable && m_parameters[i].m_expanded){
			for(index_t j = 0; j < m_parameters[i].m_subparameters.size(); ++j){
				QSize size = GetWidgetSize(m_parameters[i].m_subparameters[j].m_widget);
				y += size.height()+LAYOUT_VSPACING;

				if(pos.y() < y){
					m_current_index = i;
					m_current_subindex = j;
					break;
				}
			}
		}

		++i;
	}
}

void ParameterViewer::ExpandParameter(index_t index)
{
	if(!m_parameters[index].m_mergeable){
		m_parameters[index].m_expanded = true;

		const std::vector<Cow<ShapeInstance>>& shapes = m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->GetShapes();

		std::map<VData,size_t> values;

		for(index_t i = 0 ; i < shapes.size(); ++i) {
			const ShapeInstance &shaperef = shapes[i].Ref();
			if(shaperef.IsSelected()){
				const ShapePrototype &shaperef2 = shaperef.GetShapePrototype().Ref();

				ShapeDefinition *shape_definition = NULL; //TODO// get this from LibraryManager
				const EffectiveParameters &params = shaperef2.GetEffectiveParameters(shape_definition);

				for(index_t j = 0 ; j < params.size(); ++j) {
					if(params[j].GetName() == m_parameters[index].m_name) {
						VData value = params[j].GetValue();
						if(!values.emplace(value,1).second) {
							values[value] = values[value] + 1;
						}
					}
				}
			}

		}


		m_parameters[index].m_subparameters.clear();
		for(auto const &ent : values) {
			QLineEdit *widget = new QLineEdit(viewport());
			connect(widget,SIGNAL(editingFinished()),this,SLOT(OnParameterChange()));
			std::string str;
			Json::ToString(ent.first,str);
			widget->setText(QString::fromStdString(str));
			widget->setAutoFillBackground(true);
			widget->setBackgroundRole(QPalette::Window);
			if(widget->focusPolicy() == Qt::NoFocus)
				widget->setFocusPolicy(Qt::ClickFocus);
			widget->show();
			m_parameters[index].m_subparameters.emplace_back(ent.second,ent.first,widget);
		}
	}
}

void ParameterViewer::UnexpandParameter(index_t index)
{

	if(!m_parameters[index].m_mergeable){
		m_parameters[index].m_expanded = false;
		for(index_t i = 0; i < m_parameters[index].m_subparameters.size(); ++i) {
			m_parameters[index].m_subparameters[i].m_widget->deleteLater();
		}
		m_parameters[index].m_subparameters.clear();
	}
}

HoverRegion ParameterViewer::getHoverRegion(const QPoint &pos)
{
	positionToIndex(pos);
	if(m_current_index != INDEX_NONE && pos.x() < LAYOUT_FOLDBUTTONWIDTH+3*LAYOUT_HSPACING+LAYOUT_LABELWIDTH+LAYOUT_OVERRIDEBUTTONWIDTH){
		// OVERRIDE BUTTON
		if(pos.x() > LAYOUT_FOLDBUTTONWIDTH+3*LAYOUT_HSPACING+LAYOUT_LABELWIDTH){
			return HOVER_REGION_OVERRIDEBUTTON;
		}
		// FOLDBUTTON REGION
		else if(m_current_subindex == INDEX_NONE && pos.x() < LAYOUT_FOLDBUTTONWIDTH+LAYOUT_HSPACING){
			return HOVER_REGION_FOLDBUTTON;
		}
		// LABEL REGION
		else if(m_current_subindex == INDEX_NONE && pos.x() < LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_LABELWIDTH && pos.x() > LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING){
			return HOVER_REGION_LABEL;
		}
		// SELECT BUTTON
		else if(m_current_subindex != INDEX_NONE && pos.x() > LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING && pos.x() < LAYOUT_FOLDBUTTONWIDTH+2*LAYOUT_HSPACING+LAYOUT_SUBPARAMBUTTONWIDTH){
			return HOVER_REGION_SELECTBUTTON;
		}
		// DESELECT BUTTON
		else if(m_current_subindex != INDEX_NONE && pos.x() > LAYOUT_FOLDBUTTONWIDTH+3*LAYOUT_HSPACING+LAYOUT_SUBPARAMBUTTONWIDTH && pos.x() < LAYOUT_FOLDBUTTONWIDTH+3*LAYOUT_HSPACING+2*LAYOUT_SUBPARAMBUTTONWIDTH){
			return HOVER_REGION_DESELECTBUTTON;
		}

		//REST
		else{
			return HOVER_REGION_NONE;
		}
	}
	else{
		return HOVER_REGION_NONE;
	}
}

void ParameterViewer::changeHoverRegion(HoverRegion hover_region)
{
	if(m_hover_region != hover_region){
		m_hover_region = hover_region;
		//		viewport()->update(); //TODO only update te viewport when something needs to change
	}
	viewport()->update();
}

void ParameterViewer::ensureWidgetVisible(QWidget *childWidget)
{
	QRect rect = childWidget->geometry().adjusted(0,-LAYOUT_VSPACING,0,LAYOUT_VSPACING);
	if(rect.y() < 0) {
		verticalScrollBar()->setValue(verticalScrollBar()->value() + rect.y());
	}
	if(rect.y() + rect.height() > viewport()->height()) {
		verticalScrollBar()->setValue(verticalScrollBar()->value() + rect.y() + rect.height() - viewport()->height());
	}
}

void ParameterViewer::OnParameterChange()
{
	QObject* obj = sender();
	QLineEdit* emitter = static_cast<QLineEdit*>(obj);

	if(emitter->isModified()) {
		ParameterNameValuePair param_namevalue = GetParameterNameValue(emitter);
		//Update the model

		VData newvalue;
		try {
			Json::FromString(newvalue, emitter->text().toStdString());
		} catch(Json::ParseError &ex) {
			std::cerr << "[ParameterViewer::OnParameterChange] JSON parse error: " << ex.what() << std::endl;
			return;
		}

		const std::vector<Cow<ShapeInstance>>& shapes = m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->GetShapes();
		std::vector<Cow<ShapeInstance>> new_shapes;

		// make new m_parameter
		// copy existing widgets + expanded info
		for(index_t i = 0 ; i < shapes.size(); ++i) {
			const ShapeInstance &shapeinstance = shapes[i].Ref();
			bool param_found = false;

			if(shapeinstance.IsSelected()) {
				const ShapePrototype &shapeprototype = shapeinstance.GetShapePrototype().Ref();

				ShapeDefinition *shape_definition = NULL; //TODO// get this from LibraryManager
				const EffectiveParameters &params = shapeprototype.GetEffectiveParameters(shape_definition);

				for(index_t j = 0 ; j < params.size(); ++j) {
					if(params[j].GetName() == param_namevalue.m_name && (params[j].GetValue() == param_namevalue.m_value || param_namevalue.m_value == VData("..."))) {
						param_found = true;

						const VData::Dict &old_parameters = shapeprototype.GetParameters(); //TODO// replace with transformed parameters
						VData::Dict new_parameters;
						new_parameters.Reserve(old_parameters.GetSize() + 1);

						bool found = false;
						for(index_t k = 0; k < old_parameters.GetSize(); ++k) {
							const VDataDictEntry &entry = old_parameters[k];
							if(entry.Key() == param_namevalue.m_name) {
								new_parameters.EmplaceBack(param_namevalue.m_name, newvalue);
								found = true;
							} else {
								new_parameters.EmplaceBack(entry.Key(), entry.Value());
							}
						}
						if(!found) {
							new_parameters.EmplaceBack(param_namevalue.m_name, newvalue);
						}

						Cow<ShapePrototype> new_shapeprototype(std::make_shared<ShapePrototype>(shapeprototype.GetName(), new_parameters));
						Cow<ShapeInstance> new_shapeinstance(std::make_shared<ShapeInstance>(std::move(new_shapeprototype), shapeinstance.IsSelected()));

						new_shapes.emplace_back(std::move(new_shapeinstance));

						break;
					}
				}

			}
			if(!param_found) { // if parameter changed is not in params --> copy shape
				new_shapes.emplace_back(shapes[i]);
			}

		}
		m_mainwindow->GetDocumentViewer()->GetActiveDocument()->GetDrawing()->HistoryPush(std::move(new_shapes),false);
		emitter->setModified(false);
		UpdateParameters(); //TODO FIgure out where this update should go
		//TODO focus on drawing viewer...
	}
}


