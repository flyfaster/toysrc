/*
 * MainWnd.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: onega
 */

#include "MainWnd.h"

#include <QtGui/QMessageBox>
#include <fstream>

MainWnd::MainWnd(QWidget *parent, Qt::WFlags flags) :
		QMainWindow(parent, flags) {
	this->setCentralWidget(new QWidget(this->centralWidget()));
	QGridLayout* layout = new QGridLayout();
	scene = new QGraphicsScene(); // QRect(-50, -50, 400, 200)
	view = new QGraphicsView(this->centralWidget());
	view->setScene(scene);
//	view->setDragMode(QGraphicsView::ScrollHandDrag);
	QPixmap image = QPixmap::fromImage(
			QImage("/home/onega/Pictures/download.jpg"));
	item = new QGraphicsPixmapItem(image);
	scene->addItem(item);
//	view->show();
	QPushButton* button = new QPushButton("Next", this->centralWidget());
	QObject::connect(button, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	QLabel* url = new QLabel("URL:", this->centralWidget());
	urlinput = new QLineEdit(this->centralWidget());
	layout->addWidget(url, 0, 0);
	layout->addWidget(urlinput, 0, 1);
	layout->addWidget(new QLabel("view:", this), 1, 0);
	layout->addWidget(view, 1, 1);
	layout->addWidget(button, 2, 0);

	this->centralWidget()->setLayout(layout);
}

MainWnd::~MainWnd() {
}

void MainWnd::paintEvent1(QPaintEvent*) {
	QTime now=QTime::currentTime();
	QPainter painter(this);
	painter.begin(this);
	QRect rc = QRect(10, 10, 180, 20);
	painter.drawText(rc, now.toString());
	painter.end();
	//loadFromData
}

void MainWnd::clickedSlot()
{
	QString pathstr = urlinput->text();
	std::ifstream diskfile(pathstr.toAscii() , std::ios::binary);
	diskfile.seekg (0, std::ios::end);
	 size_t length = diskfile.tellg();
	 diskfile.seekg (0, std::ios::beg);
	 uchar* buf=new uchar[length];
	 diskfile.read( (char*)buf, length);
	 QPixmap pix;
	 pix.loadFromData(buf, length, "JPG");
	 scene->height();
	 scene->width();
	 pix.height();
	 pix.width();
	 qreal marginfactor=0.95;
	 if(scene->width()*pix.height()>scene->height()*pix.width())
		 item->setPixmap(pix.scaledToHeight(scene->height()*marginfactor));
	 else
		 item->setPixmap(pix.scaledToHeight(scene->height()*marginfactor));
//	 scene->removeItem(item);
//	 delete item;
//	item = new QGraphicsPixmapItem(pix);
//	scene->addItem(item);
}
