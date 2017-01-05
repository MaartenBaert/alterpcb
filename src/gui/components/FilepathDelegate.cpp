#include "FilepathDelegate.h"
#include <iostream>

FilepathDelegate::FilepathDelegate(QObject *parent) : QItemDelegate(parent) {
}

QWidget *FilepathDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QWidget *editor = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout();
	QLineEdit *textedit = new QLineEdit();
	QPushButton *file_button = new QPushButton("...");
	layout->addWidget(textedit);
	layout->addWidget(file_button);
	editor->setLayout(layout);

	layout->setMargin(0);
	layout->setSpacing(0);
	file_button->setMaximumWidth(30);
	file_button->setMinimumWidth(30);
	connect(file_button, SIGNAL (released()), this, SLOT (openFileBrowser()));
	textedit->setFrame(false);
	textedit->setText((index.data(Qt::DisplayRole)).toString());

	QTimer::singleShot(0, textedit, SLOT(setFocus())); // have the cursor already active in Text input box
	return editor;
}

void FilepathDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	//
}

void FilepathDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	//
}

void FilepathDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

void FilepathDelegate::openFileBrowser()
{
	QFileDialog *filedialog = new QFileDialog();
	filedialog->show();
}
