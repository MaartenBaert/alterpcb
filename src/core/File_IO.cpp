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
#include <fstream>
#include "StringRegistry.h"
#include "LayerManager.h"
#include "Library.h"
#include "LibraryManager.h"
#include "ShapeTransform.h"
#include "ShapePrototype.h"
#include "ShapeInstance.h"
#include "Drawing.h"


namespace File_IO {

//***********************************************************************************************************
// ALTERPCB FILES
void ImportFileAlterPCB_AlterpcbPythonFormat(LibraryManager &library_manager,const std::string &filename) {
	std::cerr << "WARNING : using ImportFileAlterPCB_AlterpcbPythonFormat, this does not do any checking or error handling..." << std::endl;
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

//***********************************************************************************************************
// GERBER FILES
void ImportFileGerber(const std::string &filename,Drawing *drawing, stringtag_t layer) {
	std::ifstream file(filename);

	// TODO read header to see were the decimal point should be
	float mx, my;
	std::vector<Cow<ShapeInstance>> new_shapes;
	if(drawing->Changed()){
		const std::vector<Cow<ShapeInstance>>& shapes = drawing->GetShapes();
		for(index_t i = 0 ; i < shapes.size(); ++i) {
			new_shapes.emplace_back(shapes[i]);
		}
	}

	VData::List polygon_x;
	VData::List polygon_y;

	while(true)
	{
		std::string line;
		getline( file, line );

		if(line == ""){ break; }
		if(line == "G36*" ){

			//flush polygon
			if(polygon_x.size() > 1) {
				VData::Dict params;
				params.EmplaceBack(StringRegistry::NewTag("layer"), SRGetString(layer));
				params.EmplaceBack(StringRegistry::NewTag("x"), polygon_x);
				params.EmplaceBack(StringRegistry::NewTag("y"), polygon_y);

				Cow<ShapePrototype> proto;
				proto.New(SRNewTag("polygon"), std::move(params));

				ShapeTransform transform;

				new_shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto), transform, true)); // TODO dont make selected
			}
			polygon_x.clear();
			polygon_y.clear();
		}
		else if(line == "G37*" ){

			//flush polygon
			if(polygon_x.size() > 1) {
				VData::Dict params;
				params.EmplaceBack(StringRegistry::NewTag("layer"), SRGetString(layer));
				params.EmplaceBack(StringRegistry::NewTag("x"), polygon_x);
				params.EmplaceBack(StringRegistry::NewTag("y"), polygon_y);

				Cow<ShapePrototype> proto;
				proto.New(SRNewTag("polygon"), std::move(params));

				ShapeTransform transform;

				new_shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto), transform, true)); // TODO dont make selected
			}
			polygon_x.clear();
			polygon_y.clear();
		}
		else{
			if((line != "") && (line.at(0) == 'X')){
				unsigned int pos = 1;
				while(pos < line.length() && isdigit(line.at(pos))){ pos++; }
				mx = std::stof(line.substr(1,pos-1)) / 10000;
				line = line.substr(pos,line.length()-pos);
			}
			if((line != "") && (line.at(0) == 'Y')){
				unsigned int pos = 1;
				while(pos < line.length() && isdigit(line.at(pos))){ pos++; }
				my = std::stof(line.substr(1,pos-1)) / 10000;
				line = line.substr(pos,line.length()-pos);
			}
			if(line == "D02*"){
				//flush polygon
				if(polygon_x.size() > 1) {
					VData::Dict params;
					params.EmplaceBack(StringRegistry::NewTag("layer"), SRGetString(layer));
					params.EmplaceBack(StringRegistry::NewTag("x"), polygon_x);
					params.EmplaceBack(StringRegistry::NewTag("y"), polygon_y);

					Cow<ShapePrototype> proto;
					proto.New(SRNewTag("polygon"), std::move(params));

					ShapeTransform transform;

					new_shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto), transform, true)); // TODO dont make selected
				}
				polygon_x.clear();
				polygon_y.clear();
				polygon_x.emplace_back(mx);
				polygon_y.emplace_back(my);
			}
			else if(line == "D01*"){
				polygon_x.emplace_back(mx);
				polygon_y.emplace_back(my);
			}
			else if(line != ""){
				std::cerr << "ImportFileGerber: Ignoring : "<< line << std::endl;
			}
		}
	}

	//flush polygon
	if(polygon_x.size() > 1) {
		VData::Dict params;
		params.EmplaceBack(StringRegistry::NewTag("layer"), SRGetString(layer));
		params.EmplaceBack(StringRegistry::NewTag("x"), polygon_x);
		params.EmplaceBack(StringRegistry::NewTag("y"), polygon_y);

		Cow<ShapePrototype> proto;
		proto.New(SRNewTag("polygon"), std::move(params));

		ShapeTransform transform;

		new_shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto), transform, true)); // TODO dont make selected
	}
	polygon_x.clear();
	polygon_y.clear();

	drawing->HistoryPush(std::move(new_shapes),false);
}

void ImportFileDrill(const std::__cxx11::string &filename, Drawing *drawing, stringtag_t layer)
{
	std::ifstream file(filename);


	float mx, my;

	std::vector<Cow<ShapeInstance>> new_shapes;
	if(drawing->Changed()){
		const std::vector<Cow<ShapeInstance>>& shapes = drawing->GetShapes();
		for(index_t i = 0 ; i < shapes.size(); ++i) {
			new_shapes.emplace_back(shapes[i]);
		}
	}

	int tool = 0; // Tools cant have number 0 so tool = 0 is equal no tool
	std::vector<int> tools;
	float diameter;
	std::vector<float> diameters;


	while(true)
	{
		std::string line;
		getline( file, line );
		bool flash = false;

		if(line == ""){ break; }
		if((line != "") && (line.at(0) == 'T')) {
			unsigned int pos = 1;
			while(pos < line.length() && isdigit(line.at(pos))){ pos++; }

			tool = std::stoi(line.substr(1,pos-1));
			bool tool_found = false;
			for (unsigned int i =0;i<tools.size();i++){
				if(tools.at(i) ==  tool){
					pos = i;
					tool_found = true;
					break;
				}
			}

			if(!tool_found){
				tools.emplace_back(tool);
			}
			line = line.substr(pos,line.length()-pos);

		}
		if((line != "") && (line.at(0) == 'C')) {
			unsigned int pos = 1;
			while(pos < line.length() && (isdigit(line.at(pos)) || line.at(pos) == '.')){ pos++; }

			unsigned int pos2;
			diameter = std::stof(line.substr(1,pos-1));
			bool tool_found = false;
			for (unsigned int i =0;i<tools.size();i++){
				if(tools.at(i) ==  tool){
					pos2 = i;
					tool_found = true;
					break;
				}
			}

			if(tool_found){
				while(diameters.size() <= pos2){diameters.emplace_back(0);}
				diameters.at(pos2) = diameter;
			}

			line = line.substr(pos,line.length()-pos);
		}
		if((line != "") && (line.at(0) == 'X')) {
			unsigned int pos = 1;
			while(pos < line.length() && isdigit(line.at(pos))){ pos++; }
			mx = std::stof(line.substr(1,pos-1)) / 10000; // TODO check if this is correct (python code says mx = int(line[1:pos]) / 10**(pos-4))
			line = line.substr(pos,line.length()-pos);

			flash = true;
		}
		if((line != "") && (line.at(0) == 'Y')) {
			unsigned int pos = 1;
			while(pos < line.length() && isdigit(line.at(pos))){ pos++; }
			my = std::stof(line.substr(1,pos-1)) / 10000; // TODO check if this is correct (python code says mx = int(line[1:pos]) / 10**(pos-4))
			line = line.substr(pos,line.length()-pos);

			flash = true;
		}
		else if(line != ""){
			std::cerr << "ImportFileDrill: Ignoring : "<< line << std::endl;
		}

		if(flash) {
			int pos = 0;
			bool tool_found = false;
			for (unsigned int i =0;i<tools.size();i++){
				if(tools.at(i) ==  tool){
					pos = i;
					tool_found = true;
					break;
				}
			}
			float diameter = 0;
			if(tool_found){diameter = diameters.at(pos);}

			VData::Dict params;
			params.EmplaceBack(StringRegistry::NewTag("layer"), SRGetString(layer));
			params.EmplaceBack(StringRegistry::NewTag("x"), mx);
			params.EmplaceBack(StringRegistry::NewTag("y"), my);
			params.EmplaceBack(StringRegistry::NewTag("radius"), diameter/2);

			Cow<ShapePrototype> proto;
			proto.New(SRNewTag("circle"), std::move(params));

			ShapeTransform transform;

			new_shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto), transform, true)); // TODO dont make selected

		}
	}

	drawing->HistoryPush(std::move(new_shapes),false);
}

}

