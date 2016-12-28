#include "MainWindow.h"

// these are temporary, for testing:
#include "Drawing.h"
#include "Library.h"
#include "LibraryManager.h"
#include "Shape.h"
#include "StringRegistry.h"
#include "VData.h"

#include <iostream>

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

}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	StringRegistry string_registry;
	UNUSED(string_registry);

	LibraryManager library_manager;
	{
		Library *lib1 = library_manager.NewLibrary("Basic", "basic.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib2 = library_manager.NewLibrary("My PCB", "mypcb.alterlib.json", LIBRARYTYPE_JSON);
		Library *lib3 = library_manager.NewLibrary("My Scripts", "myscripts.alterlib.py", LIBRARYTYPE_PYTHON);
		lib1->NewDrawing(StringRegistry::NewTag("resistor"), DRAWINGTYPE_SYMBOL);
		lib1->NewDrawing(StringRegistry::NewTag("resistor"), DRAWINGTYPE_LAYOUT);
		lib1->NewDrawing(StringRegistry::NewTag("via"), DRAWINGTYPE_LAYOUT);
		lib1->NewDrawing(StringRegistry::NewTag("pad"), DRAWINGTYPE_LAYOUT);
		lib2->NewDrawing(StringRegistry::NewTag("toplevel"), DRAWINGTYPE_LAYOUT);
		lib2->NewDrawing(StringRegistry::NewTag("part1"), DRAWINGTYPE_LAYOUT);
		lib2->NewDrawing(StringRegistry::NewTag("part2"), DRAWINGTYPE_LAYOUT);
		lib2->NewDrawing(StringRegistry::NewTag("part3"), DRAWINGTYPE_LAYOUT);
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart1"), DRAWINGTYPE_LAYOUT);
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart2"), DRAWINGTYPE_LAYOUT);
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart3"), DRAWINGTYPE_LAYOUT);
		lib3->NewDrawing(StringRegistry::NewTag("scriptpart4"), DRAWINGTYPE_LAYOUT);
	}

	Examples1();

	MainWindow window;
	UNUSED(window);
	return app.exec();
}
