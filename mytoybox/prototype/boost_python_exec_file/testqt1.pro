#-------------------------------------------------
#
# Project created by QtCreator 2009-12-15T17:52:42
#
#-------------------------------------------------

TARGET = testqt1
TEMPLATE = app

INCLUDEPATH += /usr/include/python3.1
LIBS += -L /usr/local/lib  -lpython3.1 -lboost_python -lboost_filesystem
SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
