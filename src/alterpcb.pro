QT += core gui opengl

TARGET = alterpcb
TEMPLATE = app

INCLUDEPATH += common core gui gui/components gui/dialogs
DEPENDPATH += common core gui gui/components gui/dialogs

QMAKE_CXXFLAGS += -std=c++11 -Wconversion -Wsign-conversion -Wfloat-conversion
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG

RESOURCES +=

########## Warning: Everything below this line is auto-generated and will be overwritten! ##########

tests {

	HEADERS += \
		tests/catch.hpp

	SOURCES += \
		tests/Main.cpp \
		tests/TestDecimal.cpp \
		tests/TestJson.cpp \
		tests/TestVData.cpp

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
	common/LinkedList.h \
	common/MiscMath.h \
	common/MurmurHash.h \
	common/Qt.h \
	common/SafePointer.h \
	common/StringHelper.h \
	common/StringRegistry.h \
	common/TrackingPointer.h \
	common/VData.h \
	common/Vector.h \
	core/CoreBasics.h \
	core/Document.h \
	core/Drawing.h \
	core/File_IO.h \
	core/LayerManager.h \
	core/LayerStack.h \
	core/Library.h \
	core/LibraryManager.h \
	core/LibraryManagerConfigProxy.h \
	core/LibraryTreeItem.h \
	core/ShapeData.h \
	core/ShapeDefinition.h \
	core/ShapeInstance.h \
	core/ShapePrototype.h \
	core/ShapeTransform.h \
	gui/DocumentViewer.h \
	gui/DrawingViewer.h \
	gui/GLHelper.h \
	gui/Icons.h \
	gui/LayerViewer.h \
	gui/LibraryViewer.h \
	gui/MainWindow.h \
	gui/ParameterViewer.h \
	gui/components/FilepathDelegate.h \
	gui/components/FilepathEditor.h \
	gui/components/LayerWidget.h \
	gui/components/TabbedDocumentViewer.h \
	gui/dialogs/GerberImportDialog.h \
	gui/dialogs/LayerConfigDialog.h \
	gui/dialogs/LibraryConfigDialog.h

SOURCES += \
	common/Color.cpp \
	common/Decimal.cpp \
	common/Json.cpp \
	common/StringRegistry.cpp \
	common/VData.cpp \
	core/Document.cpp \
	core/Drawing.cpp \
	core/File_IO.cpp \
	core/LayerManager.cpp \
	core/LayerStack.cpp \
	core/Library.cpp \
	core/LibraryManager.cpp \
	core/LibraryManagerConfigProxy.cpp \
	core/ShapeDefinition.cpp \
	core/ShapeInstance.cpp \
	core/ShapePrototype.cpp \
	gui/DocumentViewer.cpp \
	gui/DrawingViewer.cpp \
	gui/GLHelper.cpp \
	gui/Icons.cpp \
	gui/LayerViewer.cpp \
	gui/LibraryTreeItem.cpp \
	gui/LibraryViewer.cpp \
	gui/MainWindow.cpp \
	gui/ParameterViewer.cpp \
	gui/components/FilepathDelegate.cpp \
	gui/components/FilepathEditor.cpp \
	gui/components/LayerWidget.cpp \
	gui/components/TabbedDocumentViewer.cpp \
	gui/dialogs/GerberImportDialog.cpp \
	gui/dialogs/LayerConfigDialog.cpp \
	gui/dialogs/LibraryConfigDialog.cpp
