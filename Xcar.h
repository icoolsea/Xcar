/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */

#include "Camclient.h"

#ifndef XCAR_H
#define XCAR_H

#include <vlc/vlc.h>

#include "CtrlComm.h"
#include "RecordScreen.h"
#include "form_km.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
//#include <QX11EmbedContainer>
#include <QLabel>
#include <QTimer>
#include <QtNetwork>

class QVBoxLayout;
class QPushButton;
class QFrame;
class QSlider;

#define POSITION_RESOLUTION 10000

class Thread : public QThread {
        Q_OBJECT
signals:
        void aSignal(int value);

protected:
        void run() {
                //  emit aSignal();

                while (true) {
                        sleep(1);

                        qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
                        int random = qrand() % 100;
                        //      emit aSignal(random);
                }
        }
};

class Player : public QMainWindow {
        Q_OBJECT

        Thread thread_;
        CtrlComm ctrlCommThread_;
        RecordScreen recordScreenThread_;

        Form_KM *carSpeed_;
        Form_KM *servoAngle_;

        QTimer testTimer;

        Ui::MainWindow *ui;

        int camMode;

        CamClient camClient_Front;
        CamClient camClient_Back;

        QTimer *poller;
        QTimer *recording;
        bool recordStatus;
        int fifo_fd;
        bool isStopRecord;

        QUdpSocket *sender;

public:
        Player(QWidget *parent = 0);
        ~Player();

protected:
        void keyPressEvent(QKeyEvent *);
        void grabScreen();
        void startRecordScreen();
        void stopRecordScreen();

public slots:
        void playFile(QString file);

        void showSpeed(int value);
        void showAngleSignal(int value);
        void showTemperature(int value);

        void showLeftPower(int value);
        void showRightPower(int value);

        void change_Speed();
        void change_recordStatus();

        void showNewImage(QImage img);
        void showNewImage_new(QByteArray ba);

        void changeCam(int mode);

        void recordScreen();

signals:
        void stopRecordSignal();
};

#endif
