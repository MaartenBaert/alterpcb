
#include "Editor.h"
#include "components/TabbedDocumentViewer.h"

Editor::Editor(QWidget *parent) : QWidget(parent)
{


	QVBoxLayout * main_layout = new QVBoxLayout(this);
	TabbedDocumentViewer * tabbedDocViewer = new TabbedDocumentViewer();

	main_layout->addWidget(tabbedDocViewer);
	main_layout->setMargin(0);

	tabbedDocViewer->setCentralWidget(new QWidget());
	tabbedDocViewer->addDocument("Test",1);
	tabbedDocViewer->addDocument("Test2",2);

}
