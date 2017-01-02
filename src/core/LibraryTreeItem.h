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

#include "CoreBasics.h"
#include "SafePointer.h"

#include <cassert>

#include <vector>

#include <QtGui>

class LibraryTreeItem : public SafeTarget<LibraryTreeItem> {

private:
	LibraryTreeItemType m_tree_item_type;

public:
	inline LibraryTreeItem(LibraryTreeItemType type) : m_tree_item_type(type) {}

	inline LibraryTreeItemType GetTreeItemType() const { return m_tree_item_type; }

};

class LibraryTreeItemsMime : public QMimeData {
	Q_OBJECT

private:
	std::vector<SafePointer<LibraryTreeItem>> m_items;

public:
	LibraryTreeItemsMime();
	~LibraryTreeItemsMime();

	inline       std::vector<SafePointer<LibraryTreeItem>>& GetItems()       { return m_items; }
	inline const std::vector<SafePointer<LibraryTreeItem>>& GetItems() const { return m_items; }

};

