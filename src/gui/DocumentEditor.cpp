
#include "DocumentEditor.h"
#include "DrawingViewer.h"
#include "components/TabbedDocumentViewer.h"
#include "LayerManager.h"
#include "LibraryManager.h"
#include "Library.h"
#include "Drawing.h"
#include "Document.h"
#include "Icons.h"
#include <iostream>

DocumentEditor::DocumentEditor(QWidget *parent, LibraryManager* library_manager, LayerManager* layer_manager) : QWidget(parent)
{
	m_library_manager = library_manager;
	m_layer_manager = layer_manager;

	QVBoxLayout * main_layout = new QVBoxLayout(this);
	m_tabbedDocViewer = new TabbedDocumentViewer();

	main_layout->addWidget(m_tabbedDocViewer);
	main_layout->setMargin(0);

	DrawingViewer *dw  = new DrawingViewer();

	m_tabbedDocViewer->setCentralWidget(dw);

	addDocument(library_manager->GetLibrary(0)->GetLayout(0));
	addDocument(library_manager->GetLibrary(0)->GetLayout(1));

	connect(m_tabbedDocViewer,SIGNAL(tabClicked(QVariant)),this,SLOT(documentChanged(QVariant)));

	m_layer_manager->setActiveDocument(m_documents[0].Get());
}

void DocumentEditor::addDocument(Drawing *drawing)
{

	TrackingPointer<Document> document(new Document(this,drawing));
	void *ptr = (void*) document.Get();
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

	m_tabbedDocViewer->addDocument(icon,QString::fromStdString(StringRegistry::GetString(drawing->GetName())),QVariant::fromValue(ptr));
}

void DocumentEditor::documentChanged(QVariant data)
{
	Document *document = (Document*) data.value<void*>();
	m_layer_manager->setActiveDocument(document);
}
