#include "LibraryManager.h"

#include "Library.h"

LibraryManager::LibraryManager() {
	// nothing
}

LibraryManager::~LibraryManager() {
	// nothing
}

Library *LibraryManager::NewLibrary(const std::string &name, const std::string &filename, LibraryType type) {
	ForwardPointer<Library> library(new Library(name, filename, type));
	Library *ptr = library.Get();
	m_libraries.emplace_back(std::move(library));
	return ptr;
}

void LibraryManager::DeleteLibrary(Library *library) {
	DeleteFromVector(m_libraries, library);
}
