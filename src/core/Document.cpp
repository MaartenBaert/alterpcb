#include "Document.h"
#include "Drawing.h"
#include "Library.h"
#include "LayerStack.h"

Document::Document(DocumentViewer *document_editor, Drawing *drawing)
{
	m_document_editor = document_editor;
	m_drawing = drawing;

	LayerStack *layerstack = GetLayerStack();
	for(index_t i = 0 ; i < layerstack->GetLogicalLayerSize(); ++i) {
		m_layerstackattributes.EmplaceBack(layerstack->GetLogicalLayer(i)->m_name,true,true);
	}
}

LayerStack *Document::GetLayerStack()
{
	return m_drawing->GetParent()->GetLayerStack(m_drawing->GetLayerStack());
}
