#include "LayerManager.h"

#include "Icons.h"
#include "MainWindow.h"

#include <iostream>

LayerManager::LayerManager(MainWindow *main_window)
{
	m_main_window = main_window;
}

void LayerManager::BeforeDocumentChange() {
	layoutAboutToBeChanged();
}

void LayerManager::AfterDocumentChange() {
	layoutChanged();
}

QModelIndex LayerManager::index(int row, int column, const QModelIndex &parent) const
{
	// Check whether the requested index (i.e. row and column) is valid. We can already check the things that don't
	// depend on the type of item we are working with. The final row count check will be done later.
	if(row < 0)
		return QModelIndex();

	// parent should always be root
	if(!parent.isValid() && GetLayerStack()) {
		if((size_t) row >= GetLayerStack()->GetLogicalLayerSize())
			return QModelIndex();
		return createIndex(row, column,GetLayerStack()->GetLogicalLayer(row));
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
	if(!parent.isValid() && GetDocument() != NULL){
		return (int) GetLayerStack()->GetLogicalLayerSize();
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

	if(GetDocument() == NULL)
		return false;

	LogicalLayer *item_ptr = (LogicalLayer*) index.internalPointer();

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
			return GetDocument()->GetLayerSelectable(item_ptr->m_name) ? Qt::Checked : Qt::Unchecked;
		}
		if(index.column() == 2){
			return GetDocument()->GetLayerVisible(item_ptr->m_name) ? Qt::Checked : Qt::Unchecked;
		}
		else{
			return QVariant();
		}
	}
	else if(role == Qt::DecorationRole && index.column() == 0){
		QPixmap pixmap(16,16);
		pixmap.fill(Qt::transparent);

		QPixmap texturemap;
		switch (item_ptr->m_texture) {
			case LAYERTEXTURE_SOLID:
				texturemap = g_icon_layermanager_layer_solid.pixmap(16,16);
				break;
			case LAYERTEXTURE_VSTRIPE_LIGHT:
				texturemap = g_icon_layermanager_layer_vstripe_light.pixmap(16,16);
				break;
		}

		QPixmap colormap(16,16);
		colormap.fill(item_ptr->m_color);

		{
			QPainter painter(&pixmap);
			painter.drawPixmap(0, 0, 16, 16, colormap);
			painter.drawPixmap(0, 0, 16, 16, texturemap);
		}

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

	if(GetDocument() == NULL)
		return false;

	if(!index.isValid())
		return false;

	LogicalLayer *item_ptr = (LogicalLayer*) index.internalPointer();
	if(role == Qt::CheckStateRole && index.column() > 0){
		if(index.column() == 1){
			layoutAboutToBeChanged();
			GetDocument()->SetLayerSelectable(item_ptr->m_name,!GetDocument()->GetLayerSelectable(item_ptr->m_name));
			layoutChanged();
			return true;
		}
		if(index.column() == 2){
			layoutAboutToBeChanged();
			GetDocument()->SetLayerVisible(item_ptr->m_name,!GetDocument()->GetLayerVisible(item_ptr->m_name));
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

LayerStack *LayerManager::GetLayerStack() const
{
	if(GetDocument() == NULL){
		return NULL;
	}
	else{
		return m_main_window->GetDocumentViewer()->GetActiveDocument()->GetLayerStack();
	}

}

Document *LayerManager::GetDocument() const
{
	return m_main_window->GetDocumentViewer()->GetActiveDocument();
}
