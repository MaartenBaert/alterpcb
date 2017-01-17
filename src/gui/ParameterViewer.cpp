#include "ParameterViewer.h"
#include "KeyValueEditor.h"

ParameterViewer::ParameterViewer(QWidget *parent) : QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setAlignment(Qt::AlignTop);
	m_layout->setContentsMargins(10,10,10,10);
	setLayout(m_layout);
	DrawData();
}

void ParameterViewer::DrawData()
{
	KeyValueEditor *keyvaluebox1 = new KeyValueEditor(this,QString("Layer"),QString("1.0"),true);
	m_layout->addWidget(keyvaluebox1);
	KeyValueEditor *keyvaluebox2 = new KeyValueEditor(this,QString("x"),QString("..."),true);
	m_layout->addWidget(keyvaluebox2);
	KeyValueEditor *keyvaluebox3 = new KeyValueEditor(this,QString("y"),QString("1.0"),false);
	m_layout->addWidget(keyvaluebox3);
	KeyValueEditor *keyvaluebox4 = new KeyValueEditor(this,QString("Radius"),QString("1.0"),false);
	m_layout->addWidget(keyvaluebox4);

	QFrame* seperator = new QFrame();
	seperator->setFrameShape(QFrame::HLine);
	m_layout->addWidget(seperator);

	KeyValueEditor *keyvaluebox5 = new KeyValueEditor(this,QString("unknown prop"),QString("1.0"),false);
	m_layout->addWidget(keyvaluebox5);
}




