#include "LayerWidget.h"
#include "LayerManager.h"

LayerWidget::LayerWidget(QWidget *parent, LayerManager *layer_manager) : QWidget(parent) {
	m_layer_mananger = layer_manager;

}


void LayerWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect rect(10, 20, LAYOUT_TOTAL_WIDTH, LAYOUT_METAL_HEIGHT);
	painter.fillRect(rect,QBrush(QColor(128, 128, 255, 255)));
	QRect rect2(10, 20+LAYOUT_METAL_HEIGHT+LAYOUT_TOTAL_SPACING, LAYOUT_TOTAL_WIDTH, LAYOUT_METAL_HEIGHT);
	painter.fillRect(rect2,QBrush(QColor(128, 128, 255, 255)));

	QRect rect3(10, 20+LAYOUT_METAL_HEIGHT+LAYOUT_TOTAL_SPACING, LAYOUT_TOTAL_WIDTH, 2*LAYOUT_METAL_HEIGHT);
	painter.fillRect(rect3,QBrush(QColor(128, 255, 255, 255)));
}
