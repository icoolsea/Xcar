/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */


#include "Xcar.h"

#include "CtrlComm.h"

#include <sys/stat.h>


//#include <QX11EmbedContainer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFrame>

#include <QDial>
#include <QLabel>
#include <QThread>

Player::Player()
: QWidget()
{
    //preparation of the vlc command
    const char * const vlc_args[] = {
              "--verbose=2", //be much more verbose then normal for debugging purpose
              "--plugin-path=C:\\vlc-0.9.9-win32\\plugins\\" };

#ifdef Q_WS_X11
    _videoWidget=new QX11EmbedContainer(this);
#else
    _videoWidget=new QFrame(this);
#endif   

    commInfo_ = new QLabel("ffff");
    commInfo2_ = new QLabel("           距离 10 (m)");
    commInfo3_ = new QLabel("           温度 32 (摄氏度)");


    QLabel *tt1 =new QLabel("电压 10 (V)");
    QLabel *tt2 =new QLabel("电压 10 (V)");
    QLabel *tt3 = new QLabel("车体角度");
    QLabel *tt4 = new QLabel("驼机角度");;


    km_ = new Form_KM(this);
    km2_ = new Form_KM(this);

    QGridLayout *topl= new  QGridLayout;

    QLabel *tt8 = new QLabel("");
        topl->addWidget(tt8, 0, 0);


    QGridLayout *vl1= new  QGridLayout;
    vl1->addWidget(km_, 0, 0);
    vl1->addWidget(tt3, 1, 0);
    vl1->addWidget(tt1, 2, 0);

//    QLabel *tt5 = new QLabel("");
//        vl1->addWidget(tt5, 0, 0);


    QGridLayout *vl2= new QGridLayout;
    vl2->addWidget(km2_,0,0);
    vl2->addWidget(tt4,1,0);
    vl2->addWidget(tt2,2,0);

//    QLabel *tt6 = new QLabel("");
//        vl1->addWidget(tt6, 0, 0);



    QVBoxLayout *vlv= new QVBoxLayout;

    vlv->addWidget(_videoWidget, 0, 0);


   // newLayout->addWidget(km_, 0, 0);
   // newLayout->addWidget(_videoWidget, 0, 1);
   // newLayout->addWidget(km2_, 0, 2);



    QHBoxLayout *hl = new QHBoxLayout;


    hl->addWidget(commInfo_,0);
    hl->addWidget(commInfo2_,1);
    hl->addWidget(commInfo3_,2);

        QGridLayout *newLayout = new QGridLayout;


  //  newLayout->addLayout(topl,0,1);
    newLayout->addLayout(vl1,0,0);
    newLayout->addLayout(vl2,0,2);
    newLayout->addLayout(vlv,0,1);

    newLayout->addLayout(hl,1,1);

    newLayout->setColumnStretch(0, 1);
    newLayout->setColumnStretch(1,3);
    newLayout->setColumnStretch(2,1);


    newLayout->setRowStretch(0,6);
    newLayout->setRowStretch(1,1);
 //   newLayout->setRowStretch(2,1);


    setLayout(newLayout);

   // QVBoxLayout *layout = new QVBoxLayout;
   // layout->addWidget(_videoWidget);
   // layout->addWidget(commInfo_,30,Qt::AlignLeft);
  //  setLayout(layout);

   // km_->setGeometry(110,120,400,255);
    //km_->show();

    connect(&testTimer,SIGNAL(timeout()),this,SLOT(change_Speed()));
    testTimer.start(10);


    _isPlaying=false;
    poller=new QTimer(this);

    //create a new libvlc instance
    _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used
//    _vlcinstance=libvlc_new(0, NULL);  //tricky calculation of the char space used

   // while (1) {};
    // Create a media player playing environement
    _mp = libvlc_media_player_new (_vlcinstance);

    // Object obj;
    QObject::connect(&thread_, SIGNAL (aSignal(int)), this, SLOT (setValueXXX(int)));
    thread_.start();

    //Serial Thread
     QObject::connect(&ctrlCommThread_, SIGNAL (sSignal(char)), this, SLOT (setValueXXX(char)));
     ctrlCommThread_.start();

    connect(poller, SIGNAL(timeout()), this, SLOT(updateInterface()));


    poller->start(100); //start timer to trigger every 100 ms the updateInterface slot
}

void Player::setValueXXX(int x)
{

    QString t = QString::number(x, 16).toUpper();

    QString tmp = "速度"" " + t + "m/s";
    commInfo_->setText(tmp);
}

//desctructor
Player::~Player()
{
    /* Stop playing */
    libvlc_media_player_stop (_mp);

    /* Free the media_player */
    libvlc_media_player_release (_mp);

    libvlc_release (_vlcinstance);
}

void Player::playFile(QString file)
{


    /* Create a new LibVLC media descriptor */
    _m = libvlc_media_new_path(_vlcinstance, file.toStdString().c_str());

 //    _m = libvlc_media_new_location (_vlcinstance, "http://192.168.8.1:8083/?action=stream");

    libvlc_media_player_set_media (_mp, _m);

    // /!\ Please note /!\
    //
    // passing the widget to the lib shows vlc at which position it should show up
    // vlc automatically resizes the video to the ´given size of the widget
    // and it even resizes it, if the size changes at the playing
    
    /* Get our media instance to use our window */
    #if defined(Q_OS_WIN)
        libvlc_media_player_set_drawable(_mp, reinterpret_cast<unsigned int>(_videoWidget->winId()), &_vlcexcep );
        //libvlc_media_player_set_hwnd(_mp, _videoWidget->winId(), &_vlcexcep ); // for vlc 1.0
    #elif defined(Q_OS_MAC)
        libvlc_media_player_set_drawable(_mp, _videoWidget->winId(), &_vlcexcep );
        //libvlc_media_player_set_agl (_mp, _videoWidget->winId(), &_vlcexcep); // for vlc 1.0
    #else //Linux
        //[20101201 Ondrej Spilka] obsolete call on libVLC >=1.1.5 
        //libvlc_media_player_set_drawable(_mp, _videoWidget->winId(), &_vlcexcep );
        //libvlc_media_player_set_xwindow(_mp, _videoWidget->winId(), &_vlcexcep ); // for vlc 1.0

     /* again note X11 handle on Linux is needed
        winID() returns X11 handle when QX11EmbedContainer us used */
     
        int windid = _videoWidget->winId();
        libvlc_media_player_set_xwindow (_mp, windid );



    /* Play */
    libvlc_media_player_play (_mp);
#endif
    _isPlaying=true;
}

void Player::changeVolume(int newVolume)
{
    libvlc_audio_set_volume (_mp,newVolume );
}

void Player::changePosition(int newPosition)
{
    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp);
    if (curMedia == NULL)
        return;

    float pos=(float)(newPosition)/(float)POSITION_RESOLUTION;
    libvlc_media_player_set_position (_mp, pos);
}

void Player::updateInterface()
{
    if(!_isPlaying)
        return;


    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp);
    if (curMedia == NULL)
        return;

    float pos=libvlc_media_player_get_position (_mp);
    int siderPos=(int)(pos*(float)(POSITION_RESOLUTION));
    //_positionSlider->setValue(siderPos);
    int volume=libvlc_audio_get_volume (_mp);
    //_volumeSlider->setValue(volume);

}


static float temp = 0.0;
void Player::change_Speed()
{
    temp = temp + 0.5;
    km_->change_Speed(temp);
}


