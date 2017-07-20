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

#include "File_IO.h"
#include "VData.h"
#include "Json.h"
#include <iostream>
#include "StringRegistry.h"
#include "LayerManager.h"
#include "Library.h"
#include "LibraryManager.h"
#include "ShapeTransform.h"
#include "ShapePrototype.h"
#include "ShapeInstance.h"
#include "Drawing.h"

namespace File_IO {


void ImportFileAlterPCB_AlterpcbPythonFormat(LibraryManager &library_manager,const std::string &filename) {
	VData data;
	Json::FromFile(data, filename);

	Library *lib = library_manager.NewLibrary(filename, filename, LIBRARYTYPE_JSON);


	if(data.GetType() == VDATA_LIST){
		const VData::List &ref = data.AsList(); // LAYOUTS
		for(size_t i = 0; i < ref.size()-1; ++i) {
			const VData::Dict &ref2 = ref[i].AsDict();

			lib->NewDrawing(StringRegistry::NewTag(ref2[ref2.Find(StringRegistry::NewTag("name"))].Value().AsString()), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));

			const VData::List &ref3 = ref2[ref2.Find(StringRegistry::NewTag("shapes"))].Value().AsList();
			std::vector<Cow<ShapeInstance>> shapes;

			for(size_t j = 0; j < ref3.size()-1; ++j) { // SHAPES
				const VData::Dict &params = ref3[j].AsDict();

				Cow<ShapePrototype> proto;
				proto.New(SRNewTag("round-shape"), std::move(params));

				ShapeTransform transform;

				shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto), transform, true)); // TODO dont make selected
			}

			lib->GetLayout(i)->HistoryPush(std::move(shapes),false);
		}
	}
}


}

