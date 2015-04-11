#ifndef VLC_ON_QT_H
#define VLC_ON_QT_H
#include <vlc/vlc.h>
#include <QtGui/QWidget>
class QVBoxLayout;
class QPushButton;
class QTimer;
class QFrame;
class QSlider;
class QMouseEvent;

#define POSITION_RESOLUTION 100
struct libvlc_event_t;
class Player : public QWidget/*QWidget*/
{
    Q_OBJECT
    QSlider *m_video_position;
    QSlider *m_volume_level;
    QFrame *m_video_wnd;
    libvlc_instance_t *m_vlcobj;
    libvlc_media_player_t *m_player;
    libvlc_media_t *m_media;
        QPoint m_mouse_pos;
public:
    Player();
    ~Player();
        static void  onVlcEvent(const libvlc_event_t * pEvt, void * pData);
protected:
        void mouseMoveEvent( QMouseEvent * event );
        void mousePressEvent( QMouseEvent * event );
        void mouseReleaseEvent( QMouseEvent * event );
        void paintEvent( QPaintEvent * event );
public slots:
    void playFile(QString file);
    void changeVolume(int newVolume);
    void changePosition(int newPosition);
        void customEvent(QEvent *e);
        void menuOpen();
private:
        int createConfigFile(QString config_path);
};
#endif
// D:\apps\Nokia\Qt\4.7.0\bin\moc.exe vlc_on_qt.h -o vlc_on_qt_moc.cpp
