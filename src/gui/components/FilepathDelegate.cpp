#include "FilepathDelegate.h"
#include "Basics.h"
#include "gui/components/FilepathEditor.h"
#include <iostream>

FilepathDelegate::FilepathDelegate(QObject *parent) : QItemDelegate(parent) {
}

QWidget *FilepathDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	UNUSED(option);
	UNUSED(index);
	FilepathEditor *editor = new FilepathEditor(parent);
	connect(editor, SIGNAL(editingFinished()),this, SLOT(commitAndClose()));
	return editor;
}

void FilepathDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	FilepathEditor *filepatheditor = static_cast<FilepathEditor*>(editor);
	filepatheditor->setText((index.data(Qt::EditRole)).toString());
}

void FilepathDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	FilepathEditor *filepatheditor = static_cast<FilepathEditor*>(editor);
	QString value = filepatheditor->getFileName();
	model->setData(index, value, Qt::EditRole);
}

void FilepathDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	UNUSED(editor);
	UNUSED(option);
	UNUSED(index);
	editor->setGeometry(option.rect);
}

void FilepathDelegate::commitAndClose()
{
	FilepathEditor *editor = qobject_cast<FilepathEditor *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

