/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */


#ifndef XCAR_H
#define XCAR_H

#include <vlc/vlc.h>

#include "CtrlComm.h"
#include "form_km.h"

#include <QtWidgets>
//#include <QX11EmbedContainer>
#include <QLabel>
#include <QTimer>

class QVBoxLayout;
class QPushButton;
class QTimer;
class QFrame;
class QSlider;


#define POSITION_RESOLUTION 10000

class Thread : public QThread
{
Q_OBJECT
signals:
    void aSignal(int value);
protected:
    void run() {
      //  emit aSignal();

        while (true) {
            sleep(1);

            qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
            int random = qrand() % 100;
            emit aSignal(random);
        }
    }
};




class Player : public QWidget
{
    Q_OBJECT
  //  QSlider *_positionSlider;
  //  QSlider *_volumeSlider;

#ifdef Q_WS_X11    
    QX11EmbedContainer *_videoWidget;
#else
    QFrame *_videoWidget;
#endif   
    QTimer *poller;
    bool _isPlaying;

    libvlc_instance_t *_vlcinstance;
    libvlc_media_player_t *_mp;
    libvlc_media_t *_m;

    //QDial *dial_;

    Thread thread_;
    CtrlComm ctrlCommThread_;


    QLabel *commInfo_;

    QLabel *commInfo2_;

    QLabel *commInfo3_;



    Form_KM *km_;
    Form_KM *km2_;

    QTimer testTimer;


public:
    Player();
    ~Player();


public slots:
    void playFile(QString file);
    void updateInterface();
    void changeVolume(int newVolume);
    void changePosition(int newPosition);
    void setValueXXX(int value);

    void change_Speed();


};

#endif
