#include "KeyValueEditor.h"
#include "FoldButton.h"
#include <iostream>


KeyValueEditor::KeyValueEditor(QWidget *parent, const QString &key, const QString &value, bool hasChildren, int key_width) : QWidget(parent)
{
	FoldButton *foldbutton = new FoldButton(this);
	if(hasChildren) {
		foldbutton->setButtonState(FoldButtonSTATE_FOLDED);
	}
	else {
		foldbutton->setButtonState(FoldButtonSTATE_DISABLED);
	}


	QLabel *keybox = new QLabel(key);
	keybox->setMinimumWidth(key_width);

	QLineEdit *valuebox = new QLineEdit(this);
	valuebox->setText(value);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(foldbutton);
	layout->addWidget(keybox);
	layout->addWidget(valuebox);
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
}
