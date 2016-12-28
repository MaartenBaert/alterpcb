#include "MainWindow.h"

const QString MainWindow::WINDOW_TITLE = "AlterPCB";

MainWindow::MainWindow() {

	setWindowTitle(WINDOW_TITLE);

	QSplitter *splitter1 = new QSplitter(Qt::Horizontal, this);
	{
		QSplitter *splitter2 = new QSplitter(Qt::Vertical, splitter1);
		{
			QTreeView *layer_viewer = new QTreeView(splitter2);
			layer_viewer->setUniformRowHeights(true);

			QTreeView *library_viewer = new QTreeView(splitter2);
			library_viewer->setUniformRowHeights(true);

			splitter2->addWidget(layer_viewer);
			splitter2->addWidget(library_viewer);
			splitter2->setStretchFactor(0, 1);
			splitter2->setStretchFactor(1, 1);
		}

		QWidget *drawing_viewer = new QWidget(splitter1);

		QTreeView *parameter_viewer = new QTreeView(splitter1);
		parameter_viewer->setUniformRowHeights(true);

		splitter1->addWidget(splitter2);
		splitter1->addWidget(drawing_viewer);
		splitter1->addWidget(parameter_viewer);
		splitter1->setStretchFactor(0, 0);
		splitter1->setStretchFactor(1, 1);
		splitter1->setStretchFactor(2, 0);
	}
	setCentralWidget(splitter1);

	QMenuBar *menubar = new QMenuBar(this);
	{
		QMenu *menu_file = menubar->addMenu("&File");
		menu_file->addAction("Test");
		menu_file->addAction("Test");
	}
	{
		QMenu *menu_edit = menubar->addMenu("&Edit");
		menu_edit->addAction("Test");
		menu_edit->addAction("Test");
		menu_edit->addAction("Test");
	}
	setMenuBar(menubar);

	QToolBar *toolbar = new QToolBar(this);
	toolbar->setMovable(false);
	toolbar->addAction("A");
	toolbar->addAction("B");
	toolbar->addAction("C");
	addToolBar(toolbar);

	showMaximized();

}

MainWindow::~MainWindow() {
	// nothing
}
