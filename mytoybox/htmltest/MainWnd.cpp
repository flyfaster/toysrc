/*
 * MainWnd.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: onega
 */

#include "MainWnd.h"

#include <QtGui/QMessageBox>
#include <fstream>
#include <iostream>
#include <memory>
#include <boost/algorithm/string/predicate.hpp>

MainWnd::MainWnd(QWidget *parent, Qt::WFlags flags) :
		QMainWindow(parent, flags) {
	this->setCentralWidget(new QWidget(this->centralWidget()));
	QGridLayout* layout = new QGridLayout();
	scene = new QGraphicsScene(); // QRect(-50, -50, 400, 200)
	view = new QGraphicsView(this->centralWidget());
	view->setScene(scene);
//	view->setDragMode(QGraphicsView::ScrollHandDrag);
	item = new QGraphicsPixmapItem();
//	item->setPixmap(image);
	scene->addItem(item);
//	view->show();
	button_next = new QPushButton("Next", this->centralWidget());
	QObject::connect(button_next, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	button_prev = new QPushButton("Previous", this->centralWidget());
	QObject::connect(button_prev, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	button_saveurl = new QPushButton("Save URL", this->centralWidget());
	QObject::connect(button_saveurl, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	button_saveimg = new QPushButton("Save Image", this->centralWidget());
	QObject::connect(button_saveimg, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	button_exclude = new QPushButton("Exclude", this->centralWidget());
	QObject::connect(button_exclude, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	QLabel* url = new QLabel("URL:", this->centralWidget());
	urlinput = new QLineEdit(this->centralWidget());
	layout->addWidget(url, 0, 0);
	layout->addWidget(urlinput, 0, 1);
	layout->addWidget(new QLabel("view:", this), 1, 0);
	layout->addWidget(button_next, 2, 0);
	layout->addWidget(button_prev, 3, 0);
	layout->addWidget(button_saveurl, 4, 0);
	layout->addWidget(button_saveimg, 5, 0);
	layout->addWidget(button_exclude, 6, 0);
	layout->addWidget(view, 1, 1,6,1);

	view->setSizePolicy(QSizePolicy::MinimumExpanding,
	                    QSizePolicy::MinimumExpanding);

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

void MainWnd::clickedSlot() {
	std::cout << __func__ << std::endl;
	QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
	if (clickedButton == button_next) {
		QString pathstr = urlinput->text();
		DisplayLocalImage(pathstr.toStdString());
		return;
	}
	if (clickedButton == button_next) {
		QString pathstr = urlinput->text();

		return;
	}

}

int MainWnd::DisplayLocalImage(const std::string& filename) {
	QPixmap pix;
	if (boost::algorithm::iends_with(filename, "JPG")) {
		std::ifstream diskfile(filename.c_str(), std::ios::binary);
		diskfile.seekg(0, std::ios::end);
		size_t length = diskfile.tellg();
		diskfile.seekg(0, std::ios::beg);
		std::unique_ptr<uchar> buf(new uchar[length]);
		diskfile.read((char*) buf.get(), length);
		pix.loadFromData(buf.get(), length, "JPG");
	} else {
		pix = QPixmap::fromImage(QImage(filename.c_str()));
	}
	std::cout << __func__ << " " << view->height() << "," << view->width()
			<< " image " << pix.height() << "x" << pix.width() << " "
			<< filename << std::endl;
	qreal marginfactor = 0.97;
	if (view->width() * pix.height() > view->height() * pix.width())
		item->setPixmap(pix.scaledToHeight(view->height() * marginfactor));
	else
		item->setPixmap(pix.scaledToHeight(view->height() * marginfactor));
	return 0;
}
