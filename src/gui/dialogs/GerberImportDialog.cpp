#include "GerberImportDialog.h"
#include "MainWindow.h"
#include "Library.h"
#include "File_IO.h"
#include <iostream>

GerberImportDialog::GerberImportDialog(MainWindow *parent) : QDialog(parent)
{
	m_parent = parent;
	setWindowTitle(tr("Import Gerber Files"));

	QVBoxLayout *layout1 = new QVBoxLayout();
	layout1->setSpacing(0);
	layout1->setContentsMargins(10,10,10,10);
	layout1->setAlignment(Qt::AlignTop);
	layout1->setSpacing(10);

	QFormLayout *layout2 = new QFormLayout;

	layout2->setSpacing(10);

	QLabel *label1 = new QLabel(tr("Target library :"));
	std::vector<std::string> names = m_parent->GetLibraryManager()->GetLibraryNames();
	QStringList libs;
	for(int i = 0; i < names.size(); ++i) {
		libs.append(QString::fromStdString(names[i]));
	}
	m_combo_libs = new QComboBox(this);
	m_combo_libs->addItems(libs);
	layout2->addRow(label1,m_combo_libs);


	QLabel *label2 = new QLabel(tr("LayerStack :"));
	QLabel *aa = new QLabel(tr("aa"));
	layout2->addRow(label2,aa);

	QLabel *label3 = new QLabel(tr("Layout name :"));
	m_lineedit = new QLineEdit();
	layout2->addRow(label3,m_lineedit);

	layout1->addItem(layout2);




	m_TableWidget = new QTableWidget(this);
	m_TableWidget->setRowCount(0);
	m_TableWidget->setColumnCount(2);
	QStringList m_TableHeader;
	m_TableHeader.append(QString("File"));
	m_TableHeader.append(QString("Layer"));
	m_TableWidget->setHorizontalHeaderLabels(m_TableHeader);
	m_TableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_TableWidget->verticalHeader()->setVisible(false);
	m_TableWidget->setShowGrid(false);

	m_TableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	layout1->addWidget(m_TableWidget);


	QHBoxLayout *layout3 = new QHBoxLayout();

	layout3->setAlignment(Qt::AlignRight);
	layout3->setSpacing(10);

	QPushButton *button_browse = new QPushButton(tr("&Browse..."));
	QPushButton *button_import = new QPushButton(tr("&Import"));
	QPushButton *button_cancel = new QPushButton(tr("&Cancel"));
	button_browse->setFixedWidth(70);
	button_import->setFixedWidth(70);
	button_cancel->setFixedWidth(70);
	//button_import->setEnabled(0);
	connect(button_browse, SIGNAL (clicked(bool)), this, SLOT (OnBrowse()));
	connect(button_import, SIGNAL (clicked(bool)), this, SLOT (OnImport()));
	connect(button_cancel, SIGNAL (clicked(bool)), this, SLOT (OnCancel()));

	QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

	layout3->addWidget(button_browse);
	layout3->addSpacerItem(spacer);
	layout3->addWidget(button_import);
	layout3->addWidget(button_cancel);
	layout1->addItem(layout3);


	setLayout(layout1);
	setMinimumSize(500,500);

	m_availablelayers.append("Copper-top");
	m_availablelayers.append("Copper-bot");
	m_availablelayers.append("Mask-top");
}

void GerberImportDialog::closeEvent(QCloseEvent *event)
{
	m_parent->CloseImportGerberDialog();
	event->accept();
}

void GerberImportDialog::OnCancel()
{
	m_parent->CloseImportGerberDialog();
	accept();
}

void GerberImportDialog::OnBrowse()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Select Gerber Files"),QDir::homePath(),tr("Gerber files (*.gbr)") ,0,QFileDialog::DontUseNativeDialog);
	if( !filenames.isEmpty() )
	{
		int pos = m_TableWidget->rowCount();
		for (int i =0;i<filenames.count();i++){

			if(!m_files.contains(filenames.at(i))) {
				m_files.append(filenames.at(i));

				m_TableWidget->setRowCount(m_TableWidget->rowCount() + 1);
				QFileInfo *f = new QFileInfo(filenames.at(i));

				m_TableWidget->setItem(pos, 0, new QTableWidgetItem(f->fileName()));

				if(m_availablelayers.size() > 0){
					QComboBox* combo_layers = new QComboBox();
					combo_layers->addItems(m_availablelayers);
					m_TableWidget->setCellWidget(pos,1,combo_layers);
				}

				pos = pos +1;
			}
		}
	}

}

void GerberImportDialog::OnImport()
{
	Library *lib = m_parent->GetLibraryManager()->GetLibrary(m_combo_libs->currentText().toStdString());
	if(lib != NULL){
		Drawing *drawing = lib->NewDrawing(StringRegistry::NewTag(m_lineedit->text().toStdString()), DRAWINGTYPE_LAYOUT,StringRegistry::NewTag("4layerpcb"));

		for (int i =0;i<m_files.count();i++){
			QComboBox* combo_layers = static_cast<QComboBox*>(m_TableWidget->cellWidget(i,1));
			std::cerr<< combo_layers->currentText().toStdString() << std::endl;
			File_IO::ImportFileGerber(m_files.at(i).toStdString(),drawing,StringRegistry::NewTag(combo_layers->currentText().toStdString()));
		}
	}

	m_parent->CloseImportGerberDialog();
	accept();
}
