#ifndef EDITOR_H
#define EDITOR_H

#include "Basics.h"
#include "TabbedDocumentViewer.h"
#include "Document.h"
#include "TrackingPointer.h"

#include <QWidget>
#include <QtGui>

class LibraryManager;
class LayerManager;
class Drawing;

class DocumentEditor : public QWidget
{
	Q_OBJECT

private:
	std::vector<TrackingPointer<Document>> m_documents;
	TabbedDocumentViewer *m_tabbedDocViewer;
	LayerManager *m_layer_manager;
	LibraryManager *m_library_manager;

public:
	explicit DocumentEditor(QWidget *parent,LibraryManager* library_manager, LayerManager* layer_manager);
	void addDocument(Drawing* drawing);

signals:

public slots:
	void documentChanged(QVariant documentptr);
};

#endif // EDITOR_H
