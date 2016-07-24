/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */

#include "ui_mainwindow.h"
#include "Xcar.h"
#include "Camclient.h"

#include "CtrlComm.h"

#include <sys/stat.h>
#include "unistd.h"


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


    //    QLabel *carAngle = new QLabel("车体角度");
        carSpeed_ = new Form_KM(this);


     //   QGridLayout *leftLayout = new  QGridLayout;
    //    leftLayout->addWidget(carSpeed_, 0, 0);
     //   leftLayout->addWidget(carAngle, 1, 0);


        servoAngle_ = new Form_KM(this);

      //  QLabel *servoAngle = new QLabel("驼机角度");;

   //     QGridLayout *rightLayout = new QGridLayout;
     //   rightLayout->addWidget(servoAngle_,0,0);
      //  rightLayout->addWidget(servoAngle,1,0);

        QGridLayout *newLayout = new QGridLayout;
        newLayout->addWidget(carSpeed_, 0 , 0);
        newLayout->addWidget(servoAngle_, 0 , 2);
        newLayout->setSpacing(550);

        newLayout->setContentsMargins(10, 100, 100, 100);

      //  newLayout->addLayout(leftLayout,0,0);
 //       newLayout->addLayout(rightLayout,0,3);


        ui->centralWidget->setLayout(newLayout);
        QPalette palette;
        palette.setBrush(QPalette::Background,QBrush(Qt::black));
        ui->centralWidget->setPalette(palette);
        ui->centralWidget->setAutoFillBackground(true);


        connect(&testTimer,SIGNAL(timeout()),this,SLOT(change_Speed()));
        testTimer.start(10);


        //QObject::connect(&thread_, SIGNAL (aSignal(int)), this, SLOT (showSpeed(int)));
        //thread_.start();

        QObject::connect(&ctrlCommThread_, SIGNAL (showSpeedSignal(float)), this, SLOT (showSpeed(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showDistanceSignal(float)), this, SLOT (showDistance(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showTemperatureSignal(float)), this, SLOT (showTemperature(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showLeftPowerSignal(float)), this, SLOT (showLeftPower(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showRightPowerSignal(float)), this, SLOT (showRightPower(float)));

        ctrlCommThread_.start();


        camClient.connectToHost(QHostAddress("192.168.8.1"), 8083);
        camClient.requestImage();

        connect(&camClient, SIGNAL(newImageReady(QImage)), this, SLOT(showNewImage(QImage)));

}

void Player::showSpeed(float x)
{
        QString tmp = QString("速度(m/s): %1").arg(x);
        ui->speedLabel->setText(tmp);
}

void Player::showDistance(float x)
{
        QString tmp = QString("距离(m): %1").arg(x);
        ui->distanceLabel->setText(tmp);
}


void Player::showTemperature(float x)
{
        QString tmp = QString("温度(摄氏度): %1").arg(x);
        ui->temperatureLabel->setText(tmp);
}

void Player::showLeftPower(float x)
{
        QString tmp = QString("电量: %1").arg(x);
        ui->leftPowerLabel->setText(tmp);
}

void Player::showRightPower(float x)
{
        QString tmp = QString("电量: %1").arg(x);
        ui->rightPowerLabel->setText(tmp);
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
        carSpeed_->change_Speed(temp);
}


void Player::showNewImage(QImage img)
{
        ui->imgLabel->setPixmap(QPixmap::fromImage(img));
}
