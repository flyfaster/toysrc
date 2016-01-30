/*
 * MainWnd.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: onega
 */

#include "mainwnd.h"
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include "main.h"

MainWnd::MainWnd(QWidget *parent, Qt::WFlags flags) :
		QMainWindow(parent, flags) {
	this->setCentralWidget(new QWidget(this->centralWidget()));
	QGridLayout* layout = new QGridLayout();
	button_next = new QPushButton("Pinyin", this->centralWidget());
	QObject::connect(button_next, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	button_clear = new QPushButton("Clear", this->centralWidget());
	QObject::connect(button_clear, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	QLabel* url = new QLabel("Chinese:", this->centralWidget());
	urlinput = new QLineEdit(this->centralWidget());
	pinyin = new QTextEdit(this->centralWidget());
	layout->addWidget(url, 0, 0);
	layout->addWidget(urlinput, 0, 1);
	layout->addWidget(new QLabel("Pinyin:", this), 1, 0);
	layout->addWidget(button_next, 2, 0);
	layout->addWidget(button_clear, 3, 0);

	layout->addWidget(pinyin, 1, 1, 3,1);

	pinyin->setSizePolicy(QSizePolicy::MinimumExpanding,
	                    QSizePolicy::MinimumExpanding);
	urlinput->installEventFilter(this);
	this->centralWidget()->setLayout(layout);
}

MainWnd::~MainWnd() {
}

void MainWnd::clickedSlot() {
	QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
	if (clickedButton == button_next) {
		QString pathstr = urlinput->text();
		std::wstring text=pathstr.toStdWString();
		std::wstring pinyinstr=MainApp::Instance()->chinese_to_pinyin(text);
		pinyin->setText(QString::fromStdWString(pinyinstr));
		return;
	}
	if (clickedButton == button_clear) {
		QString pathstr = urlinput->text();
		urlinput->clear();
		pinyin->clear();
		return;
	}
}

bool MainWnd::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

    if (event->type() == QEvent::KeyPress)
    {
        if (keyEvent->key() == Qt::Key_Return) {
        	   QApplication::postEvent(button_next,
        	                            new QKeyEvent(QEvent::KeyPress, Qt::Key_Space, 0, 0));
        	    QApplication::postEvent(button_next,
        	                            new QKeyEvent(QEvent::KeyRelease, Qt::Key_Space, 0, 0));

        	return true;
        }
            // do something
    }
    return false;
}
