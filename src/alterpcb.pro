QT += core gui opengl

TARGET = alterpcb
TEMPLATE = app

LIBS += -lGL -lGLEW

INCLUDEPATH += common core gui gui/components gui/dialogs
DEPENDPATH += common core gui gui/components gui/dialogs

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

########## Warning: Everything below this line is auto-generated and will be overwritten! ##########

tests {

	HEADERS += \
		tests/catch.hpp

	SOURCES += \
		tests/Main.cpp \
		tests/TestDecimal.cpp \
		tests/TestJson.cpp

} else {

	SOURCES += \
		main/Main.cpp

}

HEADERS += \
	common/Basics.h \
	common/Color.h \
	common/Cow.h \
	common/Decimal.h \
	common/EnumTranslator.h \
	common/HashTable.h \
	common/Json.h \
	common/MiscMath.h \
	common/MurmurHash.h \
	common/SafePointer.h \
	common/StringHelper.h \
	common/StringRegistry.h \
	common/TrackingPointer.h \
	common/VData.h \
	core/CoreBasics.h \
	core/Drawing.h \
	core/Library.h \
	core/LibraryManager.h \
	core/LibraryManagerConfigProxy.h \
	core/LibraryTreeItem.h \
	core/Shape.h \
	gui/DrawingViewer.h \
	gui/Editor.h \
	gui/GLHelper.h \
	gui/LibraryViewer.h \
	gui/MainWindow.h \
	gui/ParameterViewer.h \
	gui/components/FilepathDelegate.h \
	gui/components/FilepathEditor.h \
	gui/components/FoldButton.h \
	gui/components/KeyValueEditor.h \
	gui/components/TabbedDocumentViewer.h \
	gui/dialogs/LibraryConfigDialog.h

SOURCES += \
	common/Color.cpp \
	common/Decimal.cpp \
	common/Json.cpp \
	common/StringRegistry.cpp \
	common/VData.cpp \
	core/Drawing.cpp \
	core/Library.cpp \
	core/LibraryManager.cpp \
	core/LibraryManagerConfigProxy.cpp \
	core/Shape.cpp \
	gui/DrawingViewer.cpp \
	gui/Editor.cpp \
	gui/GLHelper.cpp \
	gui/LibraryTreeItem.cpp \
	gui/LibraryViewer.cpp \
	gui/MainWindow.cpp \
	gui/ParameterViewer.cpp \
	gui/components/FilepathDelegate.cpp \
	gui/components/FilepathEditor.cpp \
	gui/components/FoldButton.cpp \
	gui/components/KeyValueEditor.cpp \
	gui/components/TabbedDocumentViewer.cpp \
	gui/dialogs/LibraryConfigDialog.cpp
