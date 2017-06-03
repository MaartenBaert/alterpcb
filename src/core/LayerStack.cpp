#include "LayerStack.h"

LayerStack::LayerStack(stringtag_t name)
{
	m_name = name;
}

void LayerStack::AddLayer(LogicalLayer logical_layer)
{
	m_logical_layers.push_back(logical_layer);
}
