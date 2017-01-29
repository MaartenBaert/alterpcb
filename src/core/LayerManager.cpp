#include "LayerManager.h"
#include <iostream>

LayerManager::LayerManager()
{
	m_layers.emplace_back(new LAYER(StringRegistry::NewTag("Copper-Top"),true,true,QColor(Qt::red),LAYERTEXTURE_SOLID));
	m_layers.emplace_back(new LAYER(StringRegistry::NewTag("Copper-Bottom"),false,false,QColor(Qt::blue),LAYERTEXTURE_SOLID));
	m_layers.emplace_back(new LAYER(StringRegistry::NewTag("SolderMask-Top"),true,true,QColor(Qt::yellow),LAYERTEXTURE_SOLID));
	m_layers.emplace_back(new LAYER(StringRegistry::NewTag("SolderMask-Bottom"),false,false,QColor(Qt::green),LAYERTEXTURE_SOLID));
	m_layers.emplace_back(new LAYER(StringRegistry::NewTag("Text-Top"),true,true,QColor(Qt::cyan),LAYERTEXTURE_SOLID));
	m_layers.emplace_back(new LAYER(StringRegistry::NewTag("Text-Bottom"),false,false,QColor(Qt::blue),LAYERTEXTURE_SOLID));
}

QModelIndex LayerManager::index(int row, int column, const QModelIndex &parent) const
{
	// Check whether the requested index (i.e. row and column) is valid. We can already check the things that don't
	// depend on the type of item we are working with. The final row count check will be done later.
	if(row < 0)
		return QModelIndex();

	// parent should always be root
	if(!parent.isValid()) {
		if((size_t) row >= m_layers.size())
			return QModelIndex();
		return createIndex(row, column,m_layers[row]);
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
	if(!parent.isValid()){
		return m_layers.size();
	}
	else{
		return 0;
	}
}


int LayerManager::columnCount(const QModelIndex &parent) const
{
	UNUSED(parent);
	return 3;
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

	LAYER *item_ptr = (LAYER*) index.internalPointer();

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
			return item_ptr->m_selectable ? Qt::Checked : Qt::Unchecked;
		}
		if(index.column() == 2){
			return item_ptr->m_displayed ? Qt::Checked : Qt::Unchecked;
		}
		else{
			return QVariant();
		}
	}
	else if(role == Qt::DecorationRole && index.column() == 0){
		QPixmap pixmap(100,100);
		pixmap.fill(item_ptr->m_color);
		return QIcon(pixmap);
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

	LAYER *item_ptr = (LAYER*) index.internalPointer();
	if(role == Qt::CheckStateRole && index.column() > 0){
			if(index.column() == 1){
				layoutAboutToBeChanged();
				item_ptr->m_selectable  = !item_ptr->m_selectable;
				layoutChanged();
				return true;
			}
			if(index.column() == 2){
				layoutAboutToBeChanged();
				item_ptr->m_displayed = !item_ptr->m_displayed;
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
	std::cerr << "active layer changed" << std::endl;
}
