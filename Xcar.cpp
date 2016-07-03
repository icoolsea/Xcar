/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */


#include "Xcar.h"

//#include <QX11EmbedContainer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFrame>

#include <QDial>

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

    _volumeSlider=new QSlider(Qt::Horizontal,this);
    _volumeSlider->setMaximum(100); //the volume is between 0 and 100
    _volumeSlider->setToolTip("Audio slider");

    // Note: if you use streaming, there is no ability to use the position slider
    _positionSlider=new QSlider(Qt::Horizontal,this); 
    _positionSlider->setMaximum(POSITION_RESOLUTION);

    dial_ = new QDial;
    //dial_->setFocusPolicy(Qt::StrongFocus);


    dial_->setRange(1, 100);
    //dial_->setGeometry(30, 30, 100, 100);
    // dial_->setInvertedControls(false);
    // dial_->setInvertedAppearance(true);


    // 显示刻度
    dial_->setNotchesVisible(true);
    // connect(dl[0],SIGNAL(valueChanged(int)),this,SLOT(changedDate()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_videoWidget);
    layout->addWidget(_positionSlider);
    layout->addWidget(_volumeSlider);
    layout->addWidget(dial_);
    setLayout(layout);

    _isPlaying=false;
    poller=new QTimer(this);

    //create a new libvlc instance
    _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used
    
    // Create a media player playing environement 
    _mp = libvlc_media_player_new (_vlcinstance);

    // Object obj;
    QObject::connect(&thread_, SIGNAL (aSignal(int)), dial_, SLOT (setValue(int)));
    thread_.start();

    //connect the two sliders to the corresponding slots (uses Qt's signal / slots technology)
    connect(poller, SIGNAL(timeout()), this, SLOT(updateInterface()));
    connect(_positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));
    connect(_volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));

    // connect(_positionSlider, SIGNAL(changedxxx(int)), dial_, SLOT(setValue(int)));



    poller->start(100); //start timer to trigger every 100 ms the updateInterface slot
}

void Player::setValueXXX(int x)
{
    printf("@@@@@@@@@@@@@@@@@@ %d\n", x);
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
    //the file has to be in one of the following formats /perhaps a little bit outdated)
    /*
    [file://]filename              Plain media file
    http://ip:port/file            HTTP URL
    ftp://ip:port/file             FTP URL
    mms://ip:port/file             MMS URL
    screen://                      Screen capture
    [dvd://][device][@raw_device]  DVD device
    [vcd://][device]               VCD device
    [cdda://][device]              Audio CD device
    udp:[[<source address>]@[<bind address>][:<bind port>]]
    */

    /* Create a new LibVLC media descriptor */
    _m = libvlc_media_new_path(_vlcinstance, file.toStdString().c_str());

//     _m = libvlc_media_new_location (_vlcinstance, "http://192.168.8.1:8083/?action=stream");

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

    #endif

    /* Play */
    libvlc_media_player_play (_mp);

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
    _positionSlider->setValue(siderPos);
    int volume=libvlc_audio_get_volume (_mp);
    _volumeSlider->setValue(volume);

}


