/*
 * MainWnd.cpp
 *
 *  Created on: Mar 18, 2017
 *      Author: onegazhang
 */
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QStandardPaths> // /usr/local/opt/qt@5.7/include/QtCore/QStandardPaths

#include <QtGui/QKeyEvent>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <thread>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <boost/version.hpp>
#include <boost/config.hpp>
#include "MainWnd.h"

MainWnd::MainWnd(QWidget *parent, Qt::WindowFlags flags) :
		QMainWindow(parent, flags) {
	this->setCentralWidget(new QWidget(this->centralWidget()));
	QGridLayout* layout = new QGridLayout();

	button_about = new QPushButton("About", this->centralWidget());
	QObject::connect(button_about, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	button_exit = new QPushButton("Exit", this->centralWidget());
	QObject::connect(button_exit, SIGNAL(clicked()),this, SLOT(clickedSlot()));

	int rowspan=1, colspan=1; int row = 0; int col = 0;
	layout->addWidget(button_about, row, col);
	col++;
	layout->addWidget(button_exit, row, col, rowspan, colspan);
	this->centralWidget()->setLayout(layout);
}

MainWnd::~MainWnd() {
}


std::string get_log_timestamp()
{
using namespace std::chrono;
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);
    std::stringstream ss;
    ss << local_tm.tm_year + 1900 
		<< std::setw(2) << std::setfill('0') << local_tm.tm_mon + 1
		<< std::setw(2) << std::setfill('0') << local_tm.tm_mday
		<< std::setw(2) << std::setfill('0') << local_tm.tm_hour
		<< std::setw(2) << std::setfill('0') << local_tm.tm_min;
    return ss.str();
}

unsigned int get_thread_count()
{
	unsigned int n = std::thread::hardware_concurrency();
	return std::max(n, (unsigned int)1);
}

void MainWnd::clickedSlot() {
        QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
        if (clickedButton == button_about) {
    std::ostringstream ss;
    ss << QCoreApplication::applicationFilePath().toStdString() << " ";
    ss << "pid " << QCoreApplication::applicationPid() << std::endl;
    ss << "Build with Compiler: " << BOOST_COMPILER << std::endl
      << "Platform: " << BOOST_PLATFORM << std::endl
      << "Library: " << BOOST_STDLIB << std::endl
	  << "Boost " << BOOST_LIB_VERSION << std::endl;
    QMessageBox::information(this,"About",ss.str().c_str());
		return;
	}
  if (clickedButton == button_exit) {
		close();
		return;
	} 
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWnd w;
    w.show();
    return a.exec();
}

#include "moc_MainWnd.cpp"

const char* notes=R"(
localhost:src onzhang$ brew install qt
localhost:src onzhang$ /usr/local/Cellar/qt/5.8.0_2/bin/moc MainWnd.h -o moc_MainWnd.cpp
localhost:src onzhang$ ls
MainWnd.cpp	MainWnd.h	moc_MainWnd.cpp

localhost:src onzhang$ LM-SJC-11005140:build onzhang$ otool -L miniqt
miniqt:
	/usr/local/opt/qt/lib/QtWidgets.framework/Versions/5/QtWidgets (compatibility version 5.8.0, current version 5.8.0)
	/usr/local/opt/qt/lib/QtGui.framework/Versions/5/QtGui (compatibility version 5.8.0, current version 5.8.0)
	/usr/local/opt/qt/lib/QtCore.framework/Versions/5/QtCore (compatibility version 5.8.0, current version 5.8.0)
	/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 307.5.0)
	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1238.60.2)
)";


