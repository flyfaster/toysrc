
#ifndef MAINWND_H_
#define MAINWND_H_
#include <QtGui/QMainWindow>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtCore/QTime>
#include <deque>
#include <string>
class MainWnd: public QMainWindow {
	Q_OBJECT
public:
	MainWnd(QWidget *parent = 0, Qt::WFlags flags = 0);
	virtual ~MainWnd();
public slots:
 void clickedSlot();
 bool eventFilter(QObject *obj, QEvent *event);
private:
 QLineEdit* urlinput;
 QTextEdit* pinyin;
 QPushButton* button_next;
 QPushButton* button_clear;
};

#endif
/* MAINWND_H_ */
