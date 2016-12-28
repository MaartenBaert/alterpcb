#pragma once

#include "Basics.h"

#include <QtGui>

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	static const QString WINDOW_TITLE;

private:
	// member variables here

public:
	MainWindow();
	~MainWindow();

};
