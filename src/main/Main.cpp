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

#include "Basics.h"
#include "MainWindow.h"
#include "StringRegistry.h"

// these are temporary, for testing:
#include "Drawing.h"
#include "Icons.h"
#include "Json.h"
#include "File_IO.h"
#include "LayerManager.h"
#include "LayerStack.h"
#include "Library.h"
#include "LibraryManager.h"
#include "ShapeDefinition.h"
#include "ShapeInstance.h"
#include "ShapePrototype.h"
#include "VData.h"

// these are temporary, for testing:
#include <cfenv>
#include <cfloat>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

void Examples1() {

	// VData::List = std::vector<VData>
	// VData::Dict = HashTable<VDataDictEntry, VDataDictHasher>

	// The normal way to create lists:
	VData a;
	{
		VData::List &ref = a.NewList();
		ref.reserve(5); // size is optional, this reserves memory for efficiency
		ref.emplace_back(nullptr);
		ref.emplace_back(true);
		ref.emplace_back(42);
		ref.emplace_back(12.34);
		ref.emplace_back("Test");
	}
	std::cerr << "a = " << a << std::endl;

	// Another way to create lists:
	VData a2;
	{
		VData::List &ref = a2.NewList(5);
		ref[0] = nullptr;
		ref[1] = true;
		ref[2] = 42;
		ref[3] = 12.34;
		ref[4] = "Test";
	}
	std::cerr << "a2 = " << a2 << std::endl;

	// Yet another way to create lists:
	// Only use this for short lists because it relies on recursion.
	VData a3 = MakeVList(nullptr, true, 42, 12.34, "Test");
	std::cerr << "a3 = " << a3 << std::endl;

	// Nicer-looking way, but marginally less efficient and doesn't work with mixed types.
	VData b;
	b.NewList() = {1, 2, 3};
	std::cerr << "b = " << b << std::endl;

	// Creating string tags once in advance is much more efficient and should be done whenever possible.
	stringtag_t tag_name = StringRegistry::NewTag("name");
	stringtag_t tag_office = StringRegistry::NewTag("office");
	stringtag_t tag_age = StringRegistry::NewTag("age");
	stringtag_t tag_pi = StringRegistry::NewTag("pi");
	stringtag_t tag_extra = StringRegistry::NewTag("extra");

	// The normal way to create dicts:
	VData c;
	{
		VData::Dict &ref = c.NewDict(5); // size is optional, this reserves memory for efficiency
		ref.EmplaceBack(tag_name, "Maarten Baert");
		ref.EmplaceBack(tag_office, "B02.23");
		ref.EmplaceBack(tag_age, 24);
		ref.EmplaceBack(tag_pi, M_PI);
		ref.EmplaceBack(tag_extra, std::move(a));
		// std::move() tells the compiler that we don't need 'a' anymore, so it is OK to move the value instead of
		// copying. This is more efficient for nontrivial types. It may erase the value of 'a' though.
	}
	std::cerr << "c = " << c << std::endl;
	std::cerr << "a = " << a << std::endl;

	// Another way to create dicts:
	// Only use this for short dicts because it relies on recursion.
	VData c2 = MakeVDict(
				tag_name, "Maarten Baert",
				tag_office, "B02.23",
				tag_age, 24,
				tag_pi, M_PI,
				tag_extra, MakeVList(nullptr, true, 42, 12.34, "Test"));
	std::cerr << "c2 = " << c2 << std::endl;

	// Testing small/large numbers:
	VData d = MakeVList(
		12.34e-10, 12.34e-9, 12.34e-8, 12.34e-7, 12.34e-6, 12.34e-5, 12.34e-4,
		12.34e-3 , 12.34e-2, 12.34e-1, 12.34e+0, 12.34e+1, 12.34e+2, 12.34e+3,
		12.34e+4 , 12.34e+5, 12.34e+6, 12.34e+7, 12.34e+8, 12.34e+9, 12.34e+10,
		M_PI, 1.0e-310, 1.0e-315, 1.0e-320, 0.0 / 0.0, 1e200 * 1e200, sqrt(-1.0), log(0.0),
		1, 12, 123, 1234, -12345, -123456, -123456789, -9223372036854775807, 9223372036854775807, 0.1, 0.2, 0.3, 0.4);
	//std::cerr.precision(17);
	std::cerr << "d = " << d << std::endl;
	std::cerr << "JSON: ";
	Json::Format format;
	format.multiline = false;
	Json::ToStream(d, std::cerr.rdbuf(), format);
	std::cerr << std::endl;

}

void Examples2() {

	VData data;

	auto t1 = std::chrono::high_resolution_clock::now();
	Json::FromFile(data, "data/json-large-data/json-testdata3.json");
	auto t2 = std::chrono::high_resolution_clock::now();
	Json::ToFile(data, "data/json-large-data/json-testdata3-out.json");
	auto t3 = std::chrono::high_resolution_clock::now();

	VData data2;
	Json::FromFile(data2, "data/json-large-data/json-testdata3.json");

	//std::cerr << "Testdata1 = " << data << std::endl;
	std::cerr << "JSON read time=" << std::chrono::duration<double>(t2 - t1).count() << std::endl;
	std::cerr << "JSON write time=" << std::chrono::duration<double>(t3 - t2).count() << std::endl;
	std::cerr << "Equal=" << (data == data2) << std::endl;

}

void Examples3() {

	Json::Format format;
	format.multiline = false;

	std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	//std::uniform_int_distribution<uint64_t> dist(0x0010000000000000, 0x7fefffffffffffff); // without zero and subnormals
	std::uniform_int_distribution<uint64_t> dist(0x0000000000000000, 0x7fefffffffffffff); // with zero and subnormals
	//std::uniform_int_distribution<uint64_t> dist(0x4400000000000000, 0x480fffffffffffff); // good range for finding rounding errors

	std::cerr << "Round trip testing ..." << std::endl;
	size_t n = 1000000;
	for(size_t i = 0; i < n; ++i) {
		for(size_t j = 0; j < n; ++j) {
			uint64_t val1 = dist(rng);
			val1 |= (rng() & 1) << 63;
			//val1 = 0x4510eef878a0b98b;
			VData val2 = MemCast<double>(val1);
			std::string str;
			Json::ToString(val2, str, format);
			/*
			std::string str2;
			{
				std::ostringstream ss;
				ss.precision(16);
				ss << std::scientific << val2.AsFloat();
				str2 = ss.str();
			}*/
			VData val3;
			Json::FromString(val3, str);
			uint64_t val4 = MemCast<uint64_t>(val3.AsFloat());
			if(val2.AsFloat() != val3.AsFloat() || val1 != val4) {
				uint32_t sign1 = val1 >> 63;
				uint32_t expo1 = (val1 >> 52) & ((UINT64_C(1) << 11) - 1);
				uint64_t mant1 = val1 & ((UINT64_C(1) << 52) - 1);
				uint32_t sign4 = val4 >> 63;
				uint32_t expo4 = (val4 >> 52) & ((UINT64_C(1) << 11) - 1);
				uint64_t mant4 = val4 & ((UINT64_C(1) << 52) - 1);
				std::cerr.precision(16);
				std::cerr << "fail: "
						  << std::right << sign1 << "/" << std::setw(4) << expo1 << "/" << std::setw(16) << mant1 << " >> "
						  << std::left << std::setw(25) << std::scientific << val2.AsFloat() << " >> "
						  << std::left << std::setw(25) << str << " >> "
						  << std::left << std::setw(25) << std::scientific << val3.AsFloat() << " >> "
						  << std::right << sign4 << "/" << std::setw(4) << expo4 << "/" << std::setw(16) << mant4 << std::endl;
			}
		}
		std::cerr << "Completed " << (i + 1) * n << " tests ..." << std::endl;
	}
	std::cerr << "Round trip test complete!" << std::endl;

}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	qRegisterMetaType<DocumentPointer>("DocumentPointer");

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif

	StringRegistry string_registry;
	UNUSED(string_registry);

	//Examples1();
	//Examples2();
	//Examples3();
	//return 0;

	LibraryManager library_manager;
	{
		/*
		Library *lib1 = library_manager.NewLibrary("Basic", "basic.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib2 = library_manager.NewLibrary("My PCB", "mypcb.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib3 = library_manager.NewLibrary("My Scripts", "myscripts.alterlib.py", LIBRARYTYPE_PYTHON);
		Library *lib4 = library_manager.NewLibrary("Very small pcb", "verylongpcblibname.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib5 = library_manager.NewLibrary("Random PCB", "myrandompcb.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib6 = library_manager.NewLibrary("Ugly PCB", "uglypcb.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib7 = library_manager.NewLibrary("House shaped PCB", "houseshapedpcb.alterlib.json", LIBRARYTYPE_JSON);
		UNUSED(lib7);

		lib1->NewDrawing(StringRegistry::NewTag("resistor"), DRAWINGTYPE_SYMBOL,StringRegistry::NewTag("4layerpcb"));
		lib1->NewDrawing(StringRegistry::NewTag("resistor"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));
		lib1->NewDrawing(StringRegistry::NewTag("via"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib1->NewDrawing(StringRegistry::NewTag("pad"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib2->NewDrawing(StringRegistry::NewTag("toplevel"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));
		lib2->NewDrawing(StringRegistry::NewTag("part1"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));
		lib2->NewDrawing(StringRegistry::NewTag("part2"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib2->NewDrawing(StringRegistry::NewTag("part3"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart1"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart2"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart3"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart4"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));
		lib4->NewDrawing(StringRegistry::NewTag("resistor1"), DRAWINGTYPE_SYMBOL,StringRegistry::NewTag("4layerpcb"));
		lib4->NewDrawing(StringRegistry::NewTag("resistor2"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib4->NewDrawing(StringRegistry::NewTag("resistor3"), DRAWINGTYPE_SYMBOL,StringRegistry::NewTag("4layerpcb"));
		lib4->NewDrawing(StringRegistry::NewTag("resistor4"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));
		lib5->NewDrawing(StringRegistry::NewTag("resistor5"), DRAWINGTYPE_SYMBOL,StringRegistry::NewTag("4layerpcb"));
		lib5->NewDrawing(StringRegistry::NewTag("resistor6"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));
		lib5->NewDrawing(StringRegistry::NewTag("resistor7"), DRAWINGTYPE_SYMBOL,StringRegistry::NewTag("4layerpcb"));
		lib6->NewDrawing(StringRegistry::NewTag("resistor7_v2"), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("2layerpcb"));

		LayerStack *layerstack1 = lib1->NewLayerStack(StringRegistry::NewTag("4layerpcb"));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top1"),QColor(0,255,0),LAYERTEXTURE_SOLID));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top2"),QColor(255,255,0),LAYERTEXTURE_VSTRIPE_LIGHT));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-bottom2"),QColor(0,255,255),LAYERTEXTURE_SOLID));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-bottom1"),QColor(255,0,100),LAYERTEXTURE_VSTRIPE_LIGHT));
		LayerStack *layerstack2 = lib1->NewLayerStack(StringRegistry::NewTag("2layerpcb"));
		layerstack2->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top1"),QColor(0,100,0),LAYERTEXTURE_SOLID));
		layerstack2->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top2"),QColor(0,255,50),LAYERTEXTURE_SOLID));

		VData::Dict params1;
		params1.EmplaceBack(StringRegistry::NewTag("x"), 10);
		params1.EmplaceBack(StringRegistry::NewTag("y"), 0);
		params1.EmplaceBack(StringRegistry::NewTag("radius"), 530);

		VData::Dict params2;
		params2.EmplaceBack(StringRegistry::NewTag("x"), 10);
		params2.EmplaceBack(StringRegistry::NewTag("y"), 12.5);
		params2.EmplaceBack(StringRegistry::NewTag("width"), 45);
		params2.EmplaceBack(StringRegistry::NewTag("height"), 152);

		VData::Dict params3;
		params3.EmplaceBack(StringRegistry::NewTag("x"), 10);
		params3.EmplaceBack(StringRegistry::NewTag("y"), 12.5);
		params3.EmplaceBack(StringRegistry::NewTag("width"), 45);
		params3.EmplaceBack(StringRegistry::NewTag("height"), 152);

		Cow<ShapePrototype> proto1;
		proto1.New(SRNewTag("round-shape"), std::move(params1));
		Cow<ShapePrototype> proto2;
		proto2.New(SRNewTag("square-shape"), std::move(params2));
		Cow<ShapePrototype> proto3;
		proto3.New(SRNewTag("square-shape2"), std::move(params3));

		ShapeTransform transform1;
		ShapeTransform transform2;
		ShapeTransform transform3;

		std::vector<Cow<ShapeInstance>> shapes;
		shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto1), transform1, true));
		shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto2), transform2, true));
		shapes.emplace_back(std::make_shared<ShapeInstance>(std::move(proto3), transform3, true));

		lib1->GetLayout(0)->HistoryPush(std::move(shapes),false);
		*/
		File_IO::ImportFileAlterPCB_AlterpcbPythonFormat(library_manager,"data/alterpcb_old_files-testsuite/BIAS_PCB.alterlib.json");
		File_IO::ImportFileAlterPCB_AlterpcbPythonFormat(library_manager,"data/alterpcb_old_files-testsuite/bias_adaptor.alterlib.json");

		Library *lib1 = library_manager.GetLibrary(static_cast<size_t>(0));
		LayerStack *layerstack1 = lib1->NewLayerStack(StringRegistry::NewTag("4layerpcb"));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top1"),QColor(0,255,0),LAYERTEXTURE_SOLID));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top2"),QColor(255,255,0),LAYERTEXTURE_VSTRIPE_LIGHT));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-bottom2"),QColor(0,255,255),LAYERTEXTURE_SOLID));
		layerstack1->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-bottom1"),QColor(255,0,100),LAYERTEXTURE_VSTRIPE_LIGHT));
		LayerStack *layerstack2 = lib1->NewLayerStack(StringRegistry::NewTag("2layerpcb"));
		layerstack2->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top1"),QColor(0,100,0),LAYERTEXTURE_SOLID));
		layerstack2->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top2"),QColor(0,255,50),LAYERTEXTURE_SOLID));

		Library *lib2 = library_manager.GetLibrary(static_cast<size_t>(1));
		LayerStack *layerstack3 = lib2->NewLayerStack(StringRegistry::NewTag("4layerpcb"));
		layerstack3->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top1"),QColor(0,255,0),LAYERTEXTURE_SOLID));
		layerstack3->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-top2"),QColor(255,255,0),LAYERTEXTURE_VSTRIPE_LIGHT));
		layerstack3->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-bottom2"),QColor(0,255,255),LAYERTEXTURE_SOLID));
		layerstack3->AddLayer(LogicalLayer(StringRegistry::NewTag("copper-bottom1"),QColor(255,0,100),LAYERTEXTURE_VSTRIPE_LIGHT));
	}

	LoadIcons();
	MainWindow window(&library_manager);
	UNUSED(window);
	return app.exec();
}
