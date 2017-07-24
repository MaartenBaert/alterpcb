#include "LayerStack.h"
#include "StringRegistry.h"

LayerStack::LayerStack(stringtag_t name)
{
	m_name = name;
}

std::vector<std::__cxx11::string> LayerStack::GetLayerNames()
{
	std::vector<std::string> names;
	for(unsigned int i = 0; i < m_logical_layers.size(); ++i) {
		names.emplace_back(SRGetString(m_logical_layers.at(i).m_name));
	}
	return names;
}

void LayerStack::AddLayer(LogicalLayer logical_layer)
{
	m_logical_layers.push_back(logical_layer);
}
