#include "Library.h"

#include "Drawing.h"

Library::Library(const std::string &name, const std::string &filename, LibraryType type) {

	m_name = name;
	m_file_name = filename;
	m_type = type;

}

Library::~Library() {
	// nothing
}

Drawing *Library::NewDrawing(stringtag_t name, DrawingType type) {
	ForwardPointer<Drawing> drawing(new Drawing(name, type));
	Drawing *ptr = drawing.Get();
	switch(type) {
		case DRAWINGTYPE_SCHEMATIC: m_schematics.emplace_back(std::move(drawing)); break;
		case DRAWINGTYPE_SYMBOL: m_symbols.emplace_back(std::move(drawing)); break;
		case DRAWINGTYPE_LAYOUT: m_layouts.emplace_back(std::move(drawing)); break;
	}
	return ptr;
}

void Library::DeleteDrawing(Drawing *drawing) {
	switch(drawing->GetType()) {
		case DRAWINGTYPE_SCHEMATIC: DeleteFromVector(m_schematics, drawing); break;
		case DRAWINGTYPE_SYMBOL: DeleteFromVector(m_symbols, drawing); break;
		case DRAWINGTYPE_LAYOUT: DeleteFromVector(m_layouts, drawing); break;
	}
}
