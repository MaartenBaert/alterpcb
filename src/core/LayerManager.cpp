#include "LayerManager.h"
#include "Icons.h"
#include <iostream>

LayerManager::LayerManager()
{
	m_layerstack = nullptr;
}

void LayerManager::setActiveDocument(Document *document)
{
	m_document = document;
	layoutAboutToBeChanged();
	m_layerstack = m_document->getLayerStack();
	layoutChanged();
}

QModelIndex LayerManager::index(int row, int column, const QModelIndex &parent) const
{
	// Check whether the requested index (i.e. row and column) is valid. We can already check the things that don't
	// depend on the type of item we are working with. The final row count check will be done later.
	if(row < 0)
		return QModelIndex();

	// parent should always be root
	if(!parent.isValid() && m_layerstack) {
		if((size_t) row >= m_layerstack->GetLogicLayerSize())
			return QModelIndex();
		return createIndex(row, column,m_layerstack->GetLogicLayer(row));
	}
	else{
		return QModelIndex();
	}
}

QModelIndex LayerManager::parent(const QModelIndex &index) const
{
	UNUSED(index);
	return QModelIndex();
}

int LayerManager::rowCount(const QModelIndex &parent) const
{
	UNUSED(parent);
	if(!parent.isValid() && m_layerstack){
		return m_layerstack->GetLogicLayerSize();
	}
	else{
		return 0;
	}
}


int LayerManager::columnCount(const QModelIndex &parent) const
{
	UNUSED(parent);
	if(m_layerstack){
	return 3;
	}
	else{
		return 0;
	}
}

Qt::ItemFlags LayerManager::flags(const QModelIndex &index) const
{
	if (!index.isValid())
			return 0;
	if(index.column() > 0){
		return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	}
	else{
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
}

QVariant LayerManager::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return QVariant();

	LOGIC_LAYER *item_ptr = (LOGIC_LAYER*) index.internalPointer();

	if(role == Qt::DisplayRole){
		if(index.column() == 0){
			return QString::fromStdString(StringRegistry::GetString(item_ptr->m_name));
		}
		else{
			return QVariant();
		}
	}
	else if(role == Qt::CheckStateRole && index.column() > 0){
		if(index.column() == 1){
			return m_document->m_layerstackattributes[m_document->m_layerstackattributes.Find(item_ptr->m_name)].m_selectable ? Qt::Checked : Qt::Unchecked;
		}
		if(index.column() == 2){
			return m_document->m_layerstackattributes[m_document->m_layerstackattributes.Find(item_ptr->m_name)].m_visable ? Qt::Checked : Qt::Unchecked;
		}
		else{
			return QVariant();
		}
	}
	else if(role == Qt::DecorationRole && index.column() == 0){
		QPixmap pixmap(16,16);
		pixmap.fill(Qt::transparent);
		QPixmap texturemap = QPixmap(":/layermanager-layer-vstripe");
		QPixmap colormap(16,16);
		colormap.fill(item_ptr->m_color);

		QPainter *painter= new QPainter(&pixmap);
		painter->drawPixmap(0, 0, 16, 16, colormap);
		painter->drawPixmap(0, 0, 16, 16, texturemap);
		painter->end();

		QIcon icon = QIcon(pixmap);
		return icon;
	}
	else{
		return QVariant();
	}

}

QVariant LayerManager::headerData(int section, Qt::Orientation orientation, int role) const
{
	UNUSED(section);
	UNUSED(orientation);
	UNUSED(role);
	if(role == Qt::DisplayRole){
		switch (section) {
			case 0:
				return QString(tr("Name"));
				break;
			case 1:
				return QString(tr("S"));
				break;
			case 2:
				return QString(tr("V"));
				break;
		}
	}
	return QVariant();
}

bool LayerManager::setData(const QModelIndex &index, const QVariant &value, int role)
{
	UNUSED(value);

	if(!index.isValid())
		return false;

	LOGIC_LAYER *item_ptr = (LOGIC_LAYER*) index.internalPointer();
	if(role == Qt::CheckStateRole && index.column() > 0){
			if(index.column() == 1){
				layoutAboutToBeChanged();
				m_document->m_layerstackattributes[m_document->m_layerstackattributes.Find(item_ptr->m_name)].m_selectable = !m_document->m_layerstackattributes[m_document->m_layerstackattributes.Find(item_ptr->m_name)].m_selectable;
				layoutChanged();
				return true;
			}
			if(index.column() == 2){
				layoutAboutToBeChanged();
				 m_document->m_layerstackattributes[m_document->m_layerstackattributes.Find(item_ptr->m_name)].m_visable = ! m_document->m_layerstackattributes[m_document->m_layerstackattributes.Find(item_ptr->m_name)].m_visable;
				layoutChanged();
				return true;
			}
		}
	else{
		return false;
	}

	return false;
}

void LayerManager::activeLayerChanged(QModelIndex &index)
{
	UNUSED(index);
	std::cerr << "active layer changed" << std::endl;
	layoutChanged();
}
