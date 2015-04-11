#include "vlc_on_qt.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Player p;
    p.resize(640,480);
    //p.playFile("d:\\01.avi"); //rtp://@:2626 Replace with what you want to play
    p.show();
    return a.exec();
}
