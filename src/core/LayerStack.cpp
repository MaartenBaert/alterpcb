#include "LayerStack.h"

LayerStack::LayerStack(stringtag_t name)
{
	m_name = name;
}

void LayerStack::AddLayer(LOGIC_LAYER *logic_layer)
{
	m_logic_layers.emplace_back(logic_layer);
}
