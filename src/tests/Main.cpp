#include "Basics.h"
#include "StringRegistry.h"

#include <QtGui>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif

	StringRegistry string_registry;
	UNUSED(string_registry);

	return Catch::Session().run(argc, argv);
}
