QT += core gui opengl

TARGET = alterpcb
TEMPLATE = app

LIBS += -lGL -lGLEW

INCLUDEPATH += common core gui
DEPENDPATH += common core gui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

SOURCES += \
	common/Json.cpp \
	common/StringRegistry.cpp \
	common/VData.cpp \
	core/Drawing.cpp \
	core/Library.cpp \
	core/LibraryManager.cpp \
	core/Shape.cpp \
	gui/DrawingViewer.cpp \
	gui/GLHelper.cpp \
	gui/LibraryTreeItem.cpp \
	gui/LibraryViewer.cpp \
	gui/MainWindow.cpp \
	Main.cpp

HEADERS += \
	common/BackPointer.h \
	common/Basics.h \
	common/Cow.h \
	common/EnumTranslator.h \
	common/HashTable.h \
	common/Json.h \
	common/MurmurHash.h \
	common/SafePointer.h \
	common/StringRegistry.h \
	common/VData.h \
	core/CoreBasics.h \
	core/Drawing.h \
	core/Library.h \
	core/LibraryManager.h \
	core/LibraryTreeItem.h \
	core/Shape.h \
	gui/DrawingViewer.h \
	gui/GLHelper.h \
	gui/LibraryViewer.h \
	gui/MainWindow.h
