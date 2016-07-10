/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */

#include "ui_mainwindow.h"
#include "Xcar.h"
#include "camclient.h"

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

Player::Player(QWidget *parent)
    :QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


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

    newLayout->addLayout(hl,1,1);

 //@   newLayout->setColumnStretch(0, 1);
    //newLayout->setColumnStretch(1,3);
    //newLayout->setColumnStretch(2,1);


    //newLayout->setRowStretch(0,6);
    //@newLayout->setRowStretch(1,1);
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


    poller=new QTimer(this);

    //create a new libvlc instance
   // _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used

   // while (1) {};
    // Create a media player playing environement
  //  _mp = libvlc_media_player_new (_vlcinstance);

    // Object obj;
    QObject::connect(&thread_, SIGNAL (aSignal(int)), this, SLOT (setValueXXX(int)));
    thread_.start();

    //Serial Thread
     QObject::connect(&ctrlCommThread_, SIGNAL (sSignal(char)), this, SLOT (setValueXXX(char)));
     ctrlCommThread_.start();

    connect(poller, SIGNAL(timeout()), this, SLOT(updateInterface()));

    camClient.connectToHost(QHostAddress("115.196.223.113"), 8083);
    camClient.requestImage();

    connect(&camClient, SIGNAL(newImageReady(QImage)), this, SLOT(showNewImage(QImage)));

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

}

void Player::playFile(QString file)
{

}

static float temp = 0.0;
void Player::change_Speed()
{
    temp = temp + 0.5;
    km_->change_Speed(temp);
}


void Player::showNewImage(QImage img)
{
    ui->imgLabel->setPixmap(QPixmap::fromImage(img));
}
