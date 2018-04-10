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
	std::vector<std::string> lib_names = m_parent->GetLibraryManager()->GetLibraryNames();
	QStringList libs;
	for(std::size_t i = 0; i < lib_names.size(); ++i) {
		libs.append(QString::fromStdString(lib_names[i]));
	}
	m_combo_libs = new QComboBox(this);
	m_combo_libs->addItems(libs);
	connect(m_combo_libs, SIGNAL (currentIndexChanged(const QString)), this, SLOT (OnLibChange(const QString)));
	layout2->addRow(label1,m_combo_libs);


	QLabel *label2 = new QLabel(tr("LayerStack :"));
	QStringList layerstacks;
	Library *lib = m_parent->GetLibraryManager()->GetLibrary(m_combo_libs->currentText().toStdString());
	if(lib != NULL){
		std::vector<std::string> layerstack_names = lib->GetLayerStackNames();

		for(std::size_t i = 0; i < layerstack_names.size(); ++i) {
			layerstacks.append(QString::fromStdString(layerstack_names[i]));
		}
	}
	m_combo_layerstacks = new QComboBox(this);
	m_combo_layerstacks->addItems(layerstacks);
	connect(m_combo_layerstacks, SIGNAL (currentIndexChanged(const QString)), this, SLOT (OnLayerstackChange(const QString)));
	layout2->addRow(label2,m_combo_layerstacks);


	QLabel *label3 = new QLabel(tr("Layout name :"));
	m_lineedit = new QLineEdit();
	layout2->addRow(label3,m_lineedit);
	connect(m_lineedit, SIGNAL (textChanged(const QString)), this, SLOT (OnLineEditChange(const QString)));
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
	m_button_import = new QPushButton(tr("&Import"));
	QPushButton *button_cancel = new QPushButton(tr("&Cancel"));
	button_browse->setFixedWidth(70);
	m_button_import->setFixedWidth(70);
	button_cancel->setFixedWidth(70);
	m_button_import->setEnabled(0);
	connect(button_browse, SIGNAL (clicked(bool)), this, SLOT (OnBrowse()));
	connect(m_button_import, SIGNAL (clicked(bool)), this, SLOT (OnImport()));
	connect(button_cancel, SIGNAL (clicked(bool)), this, SLOT (OnCancel()));

	QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

	layout3->addWidget(button_browse);
	layout3->addSpacerItem(spacer);
	layout3->addWidget(m_button_import);
	layout3->addWidget(button_cancel);
	layout1->addItem(layout3);


	setLayout(layout1);
	setMinimumSize(500,500);

	LayerStack *layerstack = lib->GetLayerStack(SRFindTag(m_combo_layerstacks->currentText().toStdString()));

	std::vector<std::string> layers_names = layerstack->GetLayerNames();
	m_availablelayers.clear();
	for(std::size_t i = 0; i < layers_names.size(); ++i) {
		m_availablelayers.append(QString::fromStdString(layers_names[i]));
	}
}

void GerberImportDialog::closeEvent(QCloseEvent *event)
{
	m_parent->CloseImportGerberDialog();
	event->accept();
}

void GerberImportDialog::AllowImport()
{
	// CHeck that:
	// 1) the lib is selected
	// 2) the layerstack is selected
	// 3) the drawing name is not empty
	// 4) there are file enabled and have layer selected

	bool allowimport = true;

	Library *lib = m_parent->GetLibraryManager()->GetLibrary(m_combo_libs->currentText().toStdString());
	if(lib == NULL){ allowimport = false;}

	if(m_lineedit->text() == "") { allowimport = false;}

	if(m_files.size() == 0) { allowimport = false;}


	if(allowimport) {
		m_button_import->setEnabled(true);
	}
	else {
		m_button_import->setEnabled(false);
	}
}

void GerberImportDialog::OnCancel()
{
	m_parent->CloseImportGerberDialog();
	accept();
}

void GerberImportDialog::OnBrowse()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Select Gerber Files"),QDir::homePath(),tr("Gerber files (*.gbr *.gtl *.gbl *.gts *.gbs *.gto *.gbo *.drl *.cmp *.sol *.stc *.sts *.plc *.pls *.drd *.top *.bot *.smt *.smb *.sst *.ssb)") ,0,QFileDialog::DontUseNativeDialog);
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

	AllowImport();
}

void GerberImportDialog::OnImport()
{
	Library *lib = m_parent->GetLibraryManager()->GetLibrary(m_combo_libs->currentText().toStdString());
	if(lib != NULL){
		Drawing *drawing = lib->NewDrawing(StringRegistry::NewTag(m_lineedit->text().toStdString()), DRAWINGTYPE_LAYOUT,SRFindTag(m_combo_layerstacks->currentText().toStdString()));

		for (int i =0;i<m_files.count();i++){
			QComboBox* combo_layers = static_cast<QComboBox*>(m_TableWidget->cellWidget(i,1));


			std::cerr << "A : " <<  m_files.at(i).toStdString().substr(m_files.at(i).toStdString().find_last_of(".") + 1) << std::endl;
			if(m_files.at(i).toStdString().substr(m_files.at(i).toStdString().find_last_of(".") + 1) == "drl"){
				std::cerr << "DD" << std::endl;
				File_IO::ImportFileDrill(m_files.at(i).toStdString(),drawing,SRFindTag(combo_layers->currentText().toStdString()));
			}
			else {
				File_IO::ImportFileGerber(m_files.at(i).toStdString(),drawing,SRFindTag(combo_layers->currentText().toStdString()));
			}
		}
	}

	m_parent->CloseImportGerberDialog();
	accept();
}

void GerberImportDialog::OnLibChange(const QString text)
{
	UNUSED(text);
	// TODO check the available layerstacks and reset all the layers of the files...
	QStringList layerstacks;
	Library *lib = m_parent->GetLibraryManager()->GetLibrary(m_combo_libs->currentText().toStdString());
	if(lib != NULL){
		std::vector<std::string> layerstack_names = lib->GetLayerStackNames();

		for(std::size_t i = 0; i < layerstack_names.size(); ++i) {
			layerstacks.append(QString::fromStdString(layerstack_names[i]));
		}
	}
	m_combo_layerstacks->clear();
	m_combo_layerstacks->addItems(layerstacks);

	AllowImport();
}

void GerberImportDialog::OnLayerstackChange(const QString text)
{
	UNUSED(text);
	Library *lib = m_parent->GetLibraryManager()->GetLibrary(m_combo_libs->currentText().toStdString());
	if(lib != NULL){
		LayerStack *layerstack = lib->GetLayerStack(SRFindTag(m_combo_layerstacks->currentText().toStdString()));

		std::vector<std::string> layers_names = layerstack->GetLayerNames();
		m_availablelayers.clear();
		for(std::size_t i = 0; i < layers_names.size(); ++i) {
			m_availablelayers.append(QString::fromStdString(layers_names[i]));
		}

		for(std::size_t i = 0; i < m_files.size(); ++i) {
			if(m_availablelayers.size() > 0){
				QComboBox* combo_layers = new QComboBox();
				combo_layers->addItems(m_availablelayers);

				m_TableWidget->setCellWidget(i,1,combo_layers);
			}
		}
	}
	AllowImport();
}

void GerberImportDialog::OnLineEditChange(const QString text)
{
	UNUSED(text);
	AllowImport();
}
