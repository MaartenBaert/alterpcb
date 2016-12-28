#pragma once

#include "BackPointer.h"
#include "Basics.h"
#include "CoreBasics.h"

#include <string>
#include <vector>

class Drawing;

class Library : public BackPointer<Library> {

private:
	std::string m_name, m_file_name;
	LibraryType m_type;
	std::vector<ForwardPointer<Drawing>> m_schematics;
	std::vector<ForwardPointer<Drawing>> m_symbols;
	std::vector<ForwardPointer<Drawing>> m_layouts;

public:
	Library(const std::string &name, const std::string &filename, LibraryType type);
	~Library();

	// noncopyable
	Library(const Library&) = delete;
	Library& operator=(const Library&) = delete;

	Drawing* NewDrawing(stringtag_t name, DrawingType type);
	void DeleteDrawing(Drawing *drawing);

	inline const std::string& GetName() const { return m_name; }
	inline const std::string& GetFileName() const { return m_file_name; }
	inline LibraryType GetType() const { return m_type; }

	inline Drawing* GetSchematic(size_t index) { assert(index < m_schematics.size()); return m_schematics[index].Get(); }
	inline size_t GetSchematicCount() { return m_schematics.size(); }
	inline Drawing* GetSymbol(size_t index) { assert(index < m_symbols.size()); return m_symbols[index].Get(); }
	inline size_t GetSymbolCount() { return m_symbols.size(); }
	inline Drawing* GetLayout(size_t index) { assert(index < m_layouts.size()); return m_layouts[index].Get(); }
	inline size_t GetLayoutCount() { return m_layouts.size(); }

};

struct LibraryCompare {
	inline bool operator()(const ForwardPointer<Library> &a, const ForwardPointer<Library> &b) const {
		return a->GetName() < b->GetName();
	}
};
