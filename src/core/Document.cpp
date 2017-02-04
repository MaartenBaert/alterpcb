#include "Document.h"
#include "Drawing.h"
#include "Library.h"
#include "LayerStack.h"

Document::Document(DocumentEditor *document_editor, Drawing *drawing)
{
	m_document_editor = document_editor;
	m_drawing = drawing;

	LayerStack *layerstack = getLayerStack();
	for(index_t i = 0 ; i < layerstack->GetLogicLayerSize(); ++i) {
		m_layerstackattributes.EmplaceBack(layerstack->GetLogicLayer(i)->m_name,true,true);
	}
}

LayerStack *Document::getLayerStack()
{
	return m_drawing->GetParent()->GetLayerStack(m_drawing->GetLayerStack());
}
