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

#include "TrackingPointer.h"
#include <vector>
#include <QtGui>
#include "HashTable.h"

class Drawing;
class DocumentViewer;
class LayerStack;

struct LayerStackAttributesEntry{
	stringtag_t m_name;
	bool m_visible;
	bool m_selectable;

	inline LayerStackAttributesEntry(stringtag_t name, bool visable, bool selectable)
		: m_name(name), m_visible(visable), m_selectable(selectable) {}

	inline bool operator==(const LayerStackAttributesEntry &other) const {
		return m_name == other.m_name;
	}
	inline bool operator==(stringtag_t other) const {
		return m_name == other;
	}
};

struct LayerStackAttributesHasher {
	inline hash_t operator()(hash_t hash, const LayerStackAttributesEntry &value) const {
		return MurmurHash::HashData(hash, value.m_name);
	}

	inline hash_t operator()(hash_t hash, stringtag_t value) const {
		return MurmurHash::HashData(hash, value);
	}

};

class Document : public TrackingTarget<Document>
{
private:
	Drawing *m_drawing;
	DocumentViewer *m_document_editor;
	HashTable<LayerStackAttributesEntry, LayerStackAttributesHasher> m_layerstackattributes;

public:
	Document(DocumentViewer *document_editor,Drawing *drawing);

	// noncopyable
	Document(const Document&) = delete;
	Document& operator=(const Document&) = delete;

	LayerStack *getLayerStack();
	inline Drawing *GetDrawing() {return m_drawing;}
	inline bool GetSelectable(stringtag_t str_tag) { return m_layerstackattributes[GetIndex(str_tag)].m_selectable; }
	inline void SetSelectable(stringtag_t str_tag, bool selectable) { m_layerstackattributes[GetIndex(str_tag)].m_selectable = selectable; }
	inline bool GetVisible(stringtag_t str_tag) { return m_layerstackattributes[GetIndex(str_tag)].m_visible; }
	inline void SetVisible(stringtag_t str_tag, bool visable) { m_layerstackattributes[GetIndex(str_tag)].m_visible = visable; }

private:
	inline index_t GetIndex(stringtag_t str_tag) {
		index_t index = m_layerstackattributes.Find(str_tag);
		assert(index != INDEX_NONE);
		return index;
	}

};

struct DocumentPointer {
	Document *m_ptr;

	inline DocumentPointer() {m_ptr = NULL;}
	inline DocumentPointer(Document *ptr): m_ptr(ptr) {}
};

Q_DECLARE_METATYPE(DocumentPointer)
