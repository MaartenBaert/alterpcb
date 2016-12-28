#pragma once

#include "CoreBasics.h"

class LibraryTreeItem
{
private:
	LibraryTreeItemType m_tree_item_type;

public:
	inline LibraryTreeItem(LibraryTreeItemType type){m_tree_item_type = type;};
	inline LibraryTreeItemType GetTreeItemType() {return m_tree_item_type;};
};


