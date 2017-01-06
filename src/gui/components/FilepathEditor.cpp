#include "FilepathEditor.h"
#include <iostream>

// temp includes
#include <stdlib.h>

FilepathLineEdit::FilepathLineEdit(QWidget *parent) : QLineEdit(parent) {
	connect(this, SIGNAL (textChanged(const QString)), this, SLOT (onTextChanged()));
	m_palette = new QPalette();
}

void FilepathLineEdit::onTextChanged()
{
	// TODO check if file path is correct (returns true or false)
	if (rand() % 2 == 1){
		m_palette->setColor(QPalette::Text,Qt::red);
	}
	else
	{
		m_palette->setColor(QPalette::Text,Qt::blue);
	}
	this->setPalette(*m_palette);
}


FilepathEditor::FilepathEditor(QWidget *parent) : QWidget(parent) {

	QPushButton *file_button = new QPushButton("...");
	file_button->setMaximumWidth(30);
	file_button->setMinimumWidth(30);
	connect(file_button, SIGNAL (released()), this, SLOT (openFileBrowser()));

	m_lineedit.setFrame(false);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(&m_lineedit);
	layout->addWidget(file_button);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	// Set the lineedit in focus and select all text
	QTimer::singleShot(0, &m_lineedit, SLOT(selectAll()));
	QTimer::singleShot(0, &m_lineedit, SLOT(setFocus()));
}

void FilepathEditor::setText(QString text) {
	m_lineedit.setText(text);
}


void FilepathEditor::openFileBrowser()
{
	QString file_name = QFileDialog::getOpenFileName(NULL,QString("Select Library"),"","PNG Files (*.png);;All Files (*.*)",0,QFileDialog::DontUseNativeDialog); //TODO set the correct file filter
	if(!file_name.isNull()) {
		m_lineedit.setText(file_name);
		emit editingFinished();
	}
}
