#pragma once

#include "BackPointer.h"
#include "Basics.h"
#include "CoreBasics.h"

#include <vector>

class Library;

class LibraryManager {

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

	inline Library* GetLibrary(size_t index) { assert(index < m_libraries.size()); return m_libraries[index].Get(); }
	inline size_t GetLibraryCount() { return m_libraries.size(); }

};
