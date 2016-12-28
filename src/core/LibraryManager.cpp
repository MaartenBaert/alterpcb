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
