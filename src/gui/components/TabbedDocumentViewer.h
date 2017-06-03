#ifndef TABBEDDOCUMENTVIEWER_H
#define TABBEDDOCUMENTVIEWER_H

#include "Basics.h"
#include <QWidget>
#include <QtGui>
#include <QMap>
#include <vector>
#include "Document.h"

class TabbedDocumentViewer : public QWidget
{
	Q_OBJECT

private:
	QTabBar *m_tabbar;
	QWidget *m_base;
	QRect m_panelrect;
	QVBoxLayout *m_layout;
	std::vector<Document*> m_documents;

public:
	explicit TabbedDocumentViewer(QWidget *parent = 0);
	void setCentralWidget(QWidget * central);
	void addDocument(QIcon icon, QString name, Document *ptr);
	void addDocument(QString name, Document *ptr);
	void removeDocument(Document *ptr);

private:
	void updateLayout();
	void initStyleOption(QStyleOptionTabWidgetFrame *option) const;

	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent *e);
	bool event(QEvent *ev);
	void changeEvent(QEvent *ev);

signals:
	void tabClicked(Document *ptr);


private slots:
	void tabbarClicked(int index);
};



#endif // TABBEDDOCUMENTVIEWER_H
