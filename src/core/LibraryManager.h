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

#include "BackPointer.h"
#include "Basics.h"
#include "CoreBasics.h"

#include <vector>

#include <QtGui>

class Library;

class LibraryManager : public QAbstractItemModel {
	Q_OBJECT

private:
	std::vector<ForwardPointer<Library>> m_libraries;

public:
	LibraryManager();
	~LibraryManager();

	// noncopyable
	LibraryManager(const LibraryManager&) = delete;
	LibraryManager& operator=(const LibraryManager&) = delete;

	Library* NewLibrary(const std::string &name, const std::string &filename, LibraryType type);
	void DeleteLibrary(Library *library);

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const override;
	QStringList mimeTypes() const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::DropActions supportedDropActions() const override;

private:
	void UpdatePersistentModelIndices();

public:
	inline Library* GetLibrary(size_t index) const { assert(index < m_libraries.size()); return m_libraries[index].Get(); }
	inline size_t GetLibraryCount() const { return m_libraries.size(); }

};
