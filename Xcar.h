/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */

#include "camclient.h"


#ifndef XCAR_H
#define XCAR_H

#include <vlc/vlc.h>

#include "CtrlComm.h"
#include "form_km.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
//#include <QX11EmbedContainer>
#include <QLabel>
#include <QTimer>

class QVBoxLayout;
class QPushButton;
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


class Player : public QMainWindow
{
        Q_OBJECT

        Thread thread_;
        CtrlComm ctrlCommThread_;

        Form_KM *carSpeed_;
        Form_KM *servoAngle_;

        QTimer testTimer;

        Ui::MainWindow *ui;
        CamClient camClient;

public:
        Player(QWidget *parent = 0);
        ~Player();


public slots:
        void playFile(QString file);

        void showSpeed(int value);
        void showDistance(int x);
        void showTemperature(int x);

        void showLeftPower(int x);
        void showRightPower(int x);

        void change_Speed();

        void showNewImage(QImage img);

};

#endif
