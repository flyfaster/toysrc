
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
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtCore/QTime>
#include <QtGui/QGraphicsPixmapItem>

class MainWnd: public QMainWindow {
	Q_OBJECT
public:
	MainWnd(QWidget *parent = 0, Qt::WFlags flags = 0);
	virtual ~MainWnd();
	virtual void paintEvent1(QPaintEvent*);
	QGraphicsScene* scene;
	QGraphicsView* view ;
public slots:
 void clickedSlot();
private:
 QLineEdit* urlinput;
 QGraphicsPixmapItem* item;
 QPushButton* button_next;
 QPushButton* button_prev;
 QPushButton* button_saveurl;
 QPushButton* button_saveimg;
 QPushButton* button_exclude;
 int DisplayLocalImage(const std::string& filename);
};

#endif
/* MAINWND_H_ */
