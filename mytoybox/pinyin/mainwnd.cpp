/*
 * MainWnd.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: onega
 */

#include "mainwnd.h"
#include <QtGui/QMessageBox>
#include <fstream>
#include <iostream>
#include <memory>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include <boost/locale.hpp>
#include <algorithm>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
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

	this->centralWidget()->setLayout(layout);
	LoadDict();
}

MainWnd::~MainWnd() {
}

void MainWnd::clickedSlot() {
	QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
	if (clickedButton == button_next) {
		QString pathstr = urlinput->text();
		std::wstring text=pathstr.toStdWString();
		std::wostringstream wss;
		for(auto onechar:text) {
			for(auto line: dict) {
				if (line.find(onechar)!=line.npos) {
					wss << onechar << line.substr(line.find(L'[')) << std::endl;
				}
			}
		}
		wss<<L"Note: this software does not understand polyphone!\n";
		pinyin->setText(QString::fromStdWString(wss.str()));
		return;
	}
	if (clickedButton == button_clear) {
		QString pathstr = urlinput->text();
		urlinput->clear();
		pinyin->clear();
		return;
	}

}
extern char _binary_dict_txt_start, _binary_dict_txt_size; // objdump -t dict.bin
int MainWnd::LoadDict() {
	using namespace boost::iostreams;
	basic_array_source<char> input_source(&_binary_dict_txt_start, (size_t) &_binary_dict_txt_size);
	stream<basic_array_source<char> > wifs(input_source);
    char linebuf[1024];
//    std::ifstream wifs("dict.txt");
    const char* console_encoding = "GB18030";
    while(wifs.getline(linebuf,1024)) {
    	std::string txtline = boost::locale::conv::from_utf(linebuf, console_encoding);
    	std::wstring wkey, wline;
    	std::string nkey, nline;
//    	nkey = boost::locale::conv::between((const char*)chineseword,
//    			std::string("UTF-8"), 			// to encoding
//				std::string(console_encoding)); // from encoding
//    	wkey = convert_utf8_to_utf16(nkey);
    	wline = convert_utf8_to_utf16(linebuf);
    	dict.push_back(wline);
//    	if (wline.find(wkey)!=std::string::npos)
//    	{
//    	std::cout << txtline << std::endl;
//    	std::ios::sync_with_stdio(false);
//    	std::wcout.imbue(std::locale("zh_CN.gb18030")); // locale -a | grep zh
//    	std::wcout << wline << std::endl;
//    	}
//    	// check if every line has exact 1 [ and ]
//    	if (std::count(wline.begin(), wline.end(), L'[')!=1 || std::count(wline.begin(), wline.end(), L']')!=1)
//    	{
//			std::cout << txtline << std::endl;
//			std::ios::sync_with_stdio(false);
//			std::wcout.imbue(std::locale("zh_CN.gb18030")); // locale -a | grep zh
//			std::wcout << wline << std::endl;
//    	}
    }
    return 0;
}
