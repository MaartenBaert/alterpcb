/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Basics.h"
#include "CoreBasics.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "Qt.h"
#include "TrackingPointer.h"
#include "LayerStack.h"

#include <vector>

class Library;
class Drawing;
class ShapeDefinition;
class ShapeInstance;
class ShapePrototype;

struct ShapeTrackerEntry {
	stringtag_t m_name;
	LinkedList<ShapeDefinition> m_shape_definitions;
	LinkedList<ShapeInstance> m_shape_instances;
	LinkedList<ShapePrototype> m_shape_prototypes;
	inline ShapeTrackerEntry(stringtag_t name) : m_name(name) {}
};

struct ShapeTrackerHasher {
	inline bool Equal(const ShapeTrackerEntry &a, const ShapeTrackerEntry &b) const {
		return a.m_name == b.m_name;
	}
	inline bool Equal(const ShapeTrackerEntry &a, stringtag_t b) const {
		return a.m_name == b;
	}
	inline hash_t Hash(hash_t hash, const ShapeTrackerEntry &value) const {
		return MurmurHash::HashData(hash, value.m_name);
	}
	inline hash_t Hash(hash_t hash, stringtag_t value) const {
		return MurmurHash::HashData(hash, value);
	}
};

class LibraryManager : public QAbstractItemModel {
	Q_OBJECT

private:
	std::vector<TrackingPointer<Library>> m_libraries;
	HashTable<ShapeTrackerEntry, ShapeTrackerHasher> m_shape_trackers;
	LayerStack *m_empty_layerstack;

public:
	LibraryManager();
	~LibraryManager();

	// noncopyable
	LibraryManager(const LibraryManager&) = delete;
	LibraryManager& operator=(const LibraryManager&) = delete;

	Library* NewLibrary(const std::string &name, const std::string &filename, LibraryType type);
	void DeleteLibrary(Library *library);

	void AddShapeDefinition(stringtag_t name, ShapeDefinition *shape_definition);
	void AddShapeInstance(stringtag_t name, ShapeInstance *shape_instance);
	void AddShapePrototype(stringtag_t name, ShapePrototype *shape_prototype);

public: // QAbstractItemModel interface

	// basic model access
	QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role);

	bool removeRow(int row, const QModelIndex &parent);


	// drag and drop
	Qt::DropActions supportedDropActions() const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const override;
	bool dropLocation(const QMimeData *data, Qt::DropAction action, DropLocation location, int &row, int &column, QModelIndex &parent);
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private:
	void UpdatePersistentModelIndices();

public:
	inline Library* GetLibrary(size_t index) const { assert(index < m_libraries.size()); return m_libraries[index].Get(); }
	Library* GetLibrary(std::string name);
	inline size_t GetLibraryCount() const { return m_libraries.size(); }
	inline LayerStack* GetEmplyLayerStack() {return m_empty_layerstack;}
	std::vector<std::string> GetLibraryNames();

};
