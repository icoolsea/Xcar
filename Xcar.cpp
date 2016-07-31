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
        camMode = 1;


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
        newLayout->addWidget(servoAngle_, 0 , 1);
        newLayout->setSpacing(400);

        newLayout->setContentsMargins(10, 10, 0, 0);

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
        QObject::connect(&ctrlCommThread_, SIGNAL (showAngleSignal(float)), this, SLOT (showAngleSignal(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showTemperatureSignal(float)), this, SLOT (showTemperature(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showLeftPowerSignal(float)), this, SLOT (showLeftPower(float)));
        QObject::connect(&ctrlCommThread_, SIGNAL (showRightPowerSignal(float)), this, SLOT (showRightPower(float)));

        QObject::connect(&ctrlCommThread_, SIGNAL (sendLightMode(int)), this, SLOT (changeCam(int)));

        ctrlCommThread_.start();


        camClient_Front.connectToHost(QHostAddress("192.168.1.1"), 8080);
        camClient_Back.connectToHost(QHostAddress("192.168.1.1"), 8081);
        camClient_Front.requestImage();
        camClient_Back.requestImage();

        camClient_Front.enableShow();
        camClient_Back.disableShow();


        connect(&camClient_Front, SIGNAL(newImageReady(QImage)), this, SLOT(showNewImage(QImage)));
        connect(&camClient_Back, SIGNAL(newImageReady(QImage)), this, SLOT(showNewImage(QImage)));

}

void Player::showSpeed(float x)
{
        QString tmp = QString("速度(m/s): %1").arg(x);
        ui->speedLabel->setText(tmp);
}

void Player::showAngleSignal(float x)
{
        QString tmp = QString("角度(°): %1").arg(x);
        ui->distanceLabel->setText(tmp);
}


void Player::showTemperature(float x)
{
        QString tmp = QString("温度(℃): %1").arg(x);
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

void Player::changeCam(int mode)
{
        if (mode == 1) {
                qDebug()<<"chanecam mode"<<"front ==================\n";
                camClient_Front.enableShow();
                camClient_Back.disableShow();
        }
        else if (mode == 2) {
                qDebug()<<"chanecam mode"<<"back ==================\n";
                camClient_Front.disableShow();
                camClient_Back.enableShow();
        }
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

void Player::keyPressEvent(QKeyEvent *e)
{
        if (e->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && e->key() == Qt::Key_S)
        {
                grabScreen();
                //pressed
        }
}


void Player::grabScreen()
{
        QScreen *screen = QGuiApplication::primaryScreen();
        //    sprintf(buf, "./test/%04d.jpg", i+1);
        //  screen->grabWindow(0).save(buf,"jpg");

        QPixmap pix = screen->grabWindow(0);

        QDateTime time = QDateTime::currentDateTime();
        QString dateStr = time.toString("yyyy-MM-dd_hh:mm:ss");

        QString filename = QFileDialog::getSaveFileName(this,
                                                        tr("Save Image"),
                                                        "/tmp/" + dateStr + "_",
                                                        tr("*.jpg")); //选择路径

        if(filename.isEmpty())
        {
                return;
        }
        else
        {
                if(!pix.save(filename, "jpg")) //保存图像
                {
                        QMessageBox::information(this,
                                                 tr("Failed to save the image"),
                                                 tr("保存图片失败!"));
                        return;
                }
        }
}

void Player::recordScreen()
{

}
