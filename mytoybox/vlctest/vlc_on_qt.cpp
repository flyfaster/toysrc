
#include "vlc_on_qt.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtCore/QTimer>
#include <QtGui/QFrame>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include <QtXml/QDomDocument>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <iostream>
#include <stdio.h>
const QEvent::Type VLC_EVENT_TYPE = (QEvent::Type)5001;

class QUpdatePosEvent:public QEvent
{
public:
        QUpdatePosEvent(int newPos):QEvent(VLC_EVENT_TYPE){m_newPos = newPos;}
        int m_newPos;
};
Player::Player()
: QWidget()
{
        QMenuBar* menubar=new QMenuBar;
    //preparation of the vlc command
    const char * const vlc_args[] = {
              "-I", "dummy", /* Don't use any interface */
              "--ignore-config", /* Don't use VLC's config */
              "--extraintf=logger", //log anything
              "--verbose=2", //be much more verbose then normal for debugging purpose
#if defined(Q_OS_WIN)
              "--plugin-path=D:\\apps\\VLCPortable\\App\\vlc\\plugins\\"
#else
              "--plugin-path=/usr/lib/vlc/plugins/"
#endif
          };
    m_video_wnd=new QFrame(this);
    m_volume_level=new QSlider(Qt::Horizontal,this);
    m_volume_level->setMaximum(100);
    // Note: if you use streaming, there is no ability to use the position slider
    m_video_position=new QSlider(Qt::Horizontal,this);
    m_video_position->setMaximum(POSITION_RESOLUTION);
    QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(menubar);
    layout->addWidget(m_video_wnd);
    layout->addWidget(m_video_position);
    layout->addWidget(m_volume_level);
    setLayout(layout);

    QMenu *fileMenu = new QMenu("&File");
    QAction *fileAction = new QAction("Open..", this);
    fileAction->setShortcut(Qt::CTRL + Qt::Key_O);
    fileMenu->addAction(fileAction);
    fileMenu->addSeparator();
    fileMenu->addAction("Close", QApplication::instance(), SLOT(quit()), Qt::CTRL + Qt::Key_X);
    QMenu *aboutMenu = new QMenu("&About");
    aboutMenu->addAction("About");
    connect(fileAction, SIGNAL(triggered()), this, SLOT(menuOpen()));
    menubar->addMenu(fileMenu);
    menubar->addMenu(aboutMenu);
    m_vlcobj=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    m_player = libvlc_media_player_new (m_vlcobj );
    connect(m_video_position, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));
    connect(m_volume_level, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));
    libvlc_event_manager_t * evtManager;
    evtManager = libvlc_media_player_event_manager(m_player);
    libvlc_event_attach(evtManager,
                        libvlc_MediaPlayerPositionChanged,
                        (libvlc_callback_t)onVlcEvent, this);
    libvlc_event_attach(evtManager,libvlc_MediaPlayerEndReached,
                        (libvlc_callback_t)onVlcEvent,this);
    libvlc_event_attach(evtManager,libvlc_MediaPlayerStopped,
                        (libvlc_callback_t)onVlcEvent,this);
#if defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(m_player, m_video_wnd->winId()  );
#else
    libvlc_media_player_set_xwindow(m_player, (int)(m_video_wnd->winId()) );
#endif
}

Player::~Player()
{
    libvlc_media_player_stop (m_player);
    libvlc_media_player_release (m_player);
    libvlc_release (m_vlcobj);
}

int Player::createConfigFile(QString config_path)
{
    if (QFile::exists(config_path)==false)
    {
        std::cout << __FUNCTION__ << " create sample configuration file " << (const char*)config_path.toAscii()<<std::endl;
        QFile file(config_path);
        QDomDocument doc( "qtvlc" );
        QDomElement root = doc.createElement( "qtvlc" );
        doc.appendChild( root );
        {
            QDomElement tag = doc.createElement("aspect_ratio");
            root.appendChild(tag);
            QDomText t = doc.createTextNode("4:3");
            tag.appendChild(t);
        }
        {
            QDomElement tag = doc.createElement("crop_geometry");
            root.appendChild(tag);
            QDomText t = doc.createTextNode("512x384+64+48");
            tag.appendChild(t);
        }
        if( !file.open( QIODevice::WriteOnly ) )
        {
            std::cout << __FUNCTION__ << " failed to save file " << (const char*)config_path.toAscii()<<std::endl;
            return __LINE__;
        }
        QTextStream ts( &file );
        ts << doc.toString();
        file.close();
    }
    return 0;
}

void Player::playFile(QString media_path)
{
    m_media = libvlc_media_new_path (m_vlcobj, media_path.toUtf8());
    libvlc_media_player_set_media (m_player, m_media);

    QString config_path("qtvlc.xml");
    createConfigFile(config_path);
    QFile file(config_path);
    QDomDocument doc;    
    QString errorStr;
    int errorLine =0;
    int errorCol=0;
    if(!doc.setContent(&file,true,&errorStr,&errorLine,&errorCol))
    {
        std::cout<<__FUNCTION__ << " error "<<(const char*)errorStr.toAscii()
                << " at line " << errorLine << " character " << errorCol
                <<std::endl;
        return;
    }
    {
        QDomNodeList lst = doc.elementsByTagName("aspect_ratio");
        if (lst.count()>0)
        {
            QDomNode node=lst.at(0);
            QString text=node.toElement().text();
            std::cout<<"node  "<<(const char*)node.nodeName().toAscii() << " is "
                    << (const char*)text.toAscii() << std::endl;
            std::cout<<"set "<<(const char*)node.nodeName().toAscii()<<std::endl;
            libvlc_video_set_aspect_ratio(m_player, (const char*)text.toAscii()); // 16:9
        }
    }
    {
        QDomNodeList lst = doc.elementsByTagName("crop_geometry");
        if (lst.count()>0)
        {
            QDomNode node=lst.at(0);
            QString text=node.toElement().text();
            std::cout<<"node  "<<(const char*)node.nodeName().toAscii() << " is "
                    << (const char*)text.toAscii() << std::endl;
            std::cout<<"set "<<(const char*)node.nodeName().toAscii()<<std::endl;
            libvlc_video_set_crop_geometry(m_player, (const char*)text.toAscii()); // 512x384+64+48
        }
    }
    libvlc_media_player_play (m_player );
}

void Player::changeVolume(int newVolume)
{
    libvlc_audio_set_volume (m_player,newVolume );
}

void Player::changePosition(int newPosition)
{
    libvlc_media_t *curMedia = libvlc_media_player_get_media (m_player);
    if (curMedia == NULL)
        return;
    float pos=(float)(newPosition)/(float)POSITION_RESOLUTION;
    libvlc_media_player_set_position (m_player, pos);
    raise();
}


void Player::onVlcEvent( const libvlc_event_t * pEvt, void * pData )
{
        std::cout << __FUNCTION__ << " event type " << pEvt->type << std::endl;
        int siderPos = 0;
        if(pEvt->type == libvlc_MediaPlayerPositionChanged)
        {
                if(pData)
                {
                        Player* pp = (Player*)pData;
                        int64_t iLength=libvlc_media_player_get_length(pp->m_player);
                        int64_t iTime=libvlc_media_player_get_time(pp->m_player);
                        siderPos=iLength ? static_cast<int>((static_cast<double>(iTime)/static_cast<double>(iLength) * POSITION_RESOLUTION)) : 0;
                }
        }
        if (pEvt->type == libvlc_MediaPlayerEndReached)
                siderPos = POSITION_RESOLUTION;
        QApplication::postEvent( (QWidget*)pData, new QUpdatePosEvent(siderPos));
}

void Player::customEvent( QEvent *e )
{
        if (e->type() == VLC_EVENT_TYPE)
        {
                QUpdatePosEvent* posev = (QUpdatePosEvent*)e;
                m_video_position->setValue(posev->m_newPos);
                int volume=libvlc_audio_get_volume (m_player);
                m_volume_level->setValue(volume);
        }
}

void Player::mouseMoveEvent( QMouseEvent * event )
{

}

void Player::mousePressEvent( QMouseEvent * event )
{
        m_mouse_pos = event->pos();
}

void Player::mouseReleaseEvent( QMouseEvent * event )
{
        QPoint mouse_pos2 = event->pos();
        QPoint mouse_pos1 = m_mouse_pos;
        m_mouse_pos.setX(-1);
        m_mouse_pos.setY(-1);
        // libvlc_set_crop_geometry(mp, "512x384+64+48");
        //option[0] = "--vout-filter";
        //option[1] = "crop";
        //option[2] = "--custom-crop-ratios=512x384+64+48,384x288+128+96,256x192+192+144,128x96+256+192,64x48+288+216";
        // http://forum.videolan.org/viewtopic.php?f=32&t=88913
        //libvlc_video_set_crop_geometry(m_player, "left+top+right+bottom");
}

void Player::paintEvent( QPaintEvent * event )
{
        int margin = 10;
        int topRectHeight = 50;
        QPainter painter(this);
        QColor color(10,150,100);
        painter.setBrush(QBrush(color));
        painter.drawRect(0,0,width(),height());

        QRect topRect(margin,margin,width()-2*margin,topRectHeight);
        painter.drawRect(topRect);
        painter.drawText(width()/3,topRectHeight/2+margin,"test paintEvent");

        QRect downRect(margin,topRectHeight+2*margin,width()-2*margin,
                height()-topRectHeight-3*margin);
        painter.drawRect(downRect);
}

void Player::menuOpen()
{
        std::cout << __FUNCTION__ << std::endl;
        QString file = QFileDialog::getOpenFileName(this, tr("Open video"),
                "/d4/bin/m",
                "Video (*.avi *.mp4 *.mpg)");
        if (file.length())
        {
                playFile(file);
        }
}

// libvlc_video_take_snapshot
//http://doc.qt.nokia.com/4.1/qwidget.html#transparency-and-double-buffering

// :sout="#transcode{vcodec=h264,deinterlace=1,vfilter=croppadd{cropleft=8,cropright=8},acodec=mp4a,ab=128,channels=2,samplerate=48000}:standard{access=file,dst='${1}'}
