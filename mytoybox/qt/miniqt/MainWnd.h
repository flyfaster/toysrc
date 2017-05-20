#ifndef MAINWND_H_
#define MAINWND_H_

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QApplication>
#include <QtCore/QTime>
#include <QtCore/QDebug>

class MainWnd : public QMainWindow {
public:
    Q_OBJECT
  public:
    MainWnd(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWnd();
  public slots:
    void clickedSlot();

  private:
    QPushButton *button_about;
    QPushButton *button_exit;
};

#endif /* MAINWND_H_ */
