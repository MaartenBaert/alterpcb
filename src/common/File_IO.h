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

#include <string>
#include "LibraryManager.h"

namespace File_IO {

void ImportFileAlterPCB_AlterpcbPythonFormat(LibraryManager &library_manager, const std::string &filename);
//void ExportFileAlterPCB(const std::string &filename);

//void ImportFileSVG(const std::string &filename);
//void ExportFileSVG(const std::string &filename);

void ImportFileGerber(const std::string &filename,Drawing *drawing, stringtag_t layer);
void ImportFileDrill(const std::string &filename,Drawing *drawing, stringtag_t layer);
//void ExportFileGerber(const std::string &filename);

//void ImportFileGDS(const std::string &filename);
//void ExportFileGDS(const std::string &filename);

}
