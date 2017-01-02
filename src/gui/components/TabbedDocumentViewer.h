#ifndef TABBEDDOCUMENTVIEWER_H
#define TABBEDDOCUMENTVIEWER_H

#include "Basics.h"
#include <QWidget>
#include <QtGui>
#include <QMap>

class TabbedDocumentViewer : public QWidget
{
	Q_OBJECT

private:
	QTabBar *m_tabbar;
	QWidget *m_base;
	QRect m_panelrect;
	QVBoxLayout *l;
	void updateLayout();
	void initStyleOption(QStyleOptionTabWidgetFrame *option) const;

	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent *e);
	bool event(QEvent *ev);
	void changeEvent(QEvent *ev);
	QMap<int,int> tabDocIndex;
public:
	explicit TabbedDocumentViewer(QWidget *parent = 0);
	void setCentralWidget(QWidget * central);
	void addDocument(QIcon icon, QString name, int index);
	void addDocument(QString name, int index);
	void removeDocument(int index);

signals:
	void tabClicked(int index);


private slots:
	void tabbarClicked(int index);

public slots:
};



#endif // TABBEDDOCUMENTVIEWER_H
