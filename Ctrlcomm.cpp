#include "CtrlComm.h"
#include "Log.h"

#include <QByteArray>
#include <QSettings>
#include <QtWidgets>

#include <time.h>

static const int timeOut = 10;

CtrlComm::CtrlComm() : isConnected_(false) { serial_ = new SerialThread; }

CtrlComm::~CtrlComm() {
        closeComm();
        delete serial_;
        delete carSocket_;
}

void CtrlComm::run() {

        openComm();

        carSocket_ = new QTcpSocket();

        //       carSocket_->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,
        //       0);

        connect(carSocket_, SIGNAL(connected()), this, SLOT(connectedSlot()),
                Qt::DirectConnection);
        connect(carSocket_, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()),
                Qt::DirectConnection);
        connect(carSocket_, SIGNAL(readyRead()), this, SLOT(readyReadSlot()),
                Qt::DirectConnection);
        connect(carSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this,
                SLOT(errorSlot(QAbstractSocket::SocketError)), Qt::DirectConnection);

        connect(this, SIGNAL(sendToServerSignal(QByteArray)), this,
                SLOT(sendToServer(QByteArray)), Qt::DirectConnection);

        QSettings *configIniRead = new QSettings("xcar.ini", QSettings::IniFormat);

        QString ipResult = configIniRead->value("/cmd_server/ip").toString();
        QString portResult = configIniRead->value("/cmd_server/port").toString();

        delete configIniRead;

        char tmpdata[5] = {(unsigned char)0xff, (unsigned char)0x02,
                           (unsigned char)0x02, (unsigned char)0x80,
                           (unsigned char)0xff};

        connectToServer(ipResult.toStdString().c_str(), portResult.toShort());

        while (true) {

#if 1
                if (!serial_->isOpen()) {
                        sleep(1);
                        //    emit sendToServerSignal(tmpdata);
                        LOG_ERROR << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
                        continue;
                }
                unsigned char tmp[5];
                memset(&tmp, 0x00, 5);
                int count = serial_->readData((char *)tmp, 5, timeOut);
                if (count <= 0) {
                        //        emit sendToServerSignal(tmpdata);
                        continue;
                }

                //                   LOG_ERROR<<"1 ===================================== not
                //                   find data\n ";
                //     carSocket_->write((const char*)tmp, count);
                //               LOG_ERROR<<"2 ===================================== not
                //               find data\n ";
                // carSocket_->waitForBytesWritten(timeOut);
                //           LOG_ERROR<<"3 ===================================== not find
                //           data\n ";
                //             continue;

                QByteArray okData((const char *)tmp);

                serialArray_.append(okData);
                //       LOG_DEBUG<<"[Serial], recv data: "<<serialArray_.toHex();

                // 0xFF 0xFF
                char spos[2] = {(unsigned char)0xff};
                char epos[2] = {(unsigned char)0xff};


                int startPos = serialArray_.indexOf(spos, 0);
                if (startPos == -1) {
                        LOG_ERROR << "1 not find data\n ";
                        serialArray_.clear();
                        continue;
                }

                int endPos = serialArray_.indexOf(epos, startPos + 1);
                if (endPos == -1) {
                        LOG_ERROR << "2 not find data\n ";
                        continue;
                }

                if (endPos - startPos == 1) {
                        serialArray_.clear();
                        //TODO !!! LOG_ERROR << "3 not find data\n ";
                        continue;
                }

                QByteArray serialBuf = serialArray_.mid(startPos, endPos - startPos + 1);

                if (serialBuf[1] == 0x02 &&
                                (serialBuf[3] != 0x80 || serialBuf[3] != 0x7f)) {
                        QByteArray t = serialBuf.toHex();
                        //  LOG_ERROR<<t;
                }
                serialArray_.clear();

                char light_front[5] = {(unsigned char)0xff, (unsigned char)0x03,
                                       (unsigned char)0x03, (unsigned char)0x01};
                char light_back[5] = {(unsigned char)0xff, (unsigned char)0x03,
                                      (unsigned char)0x03, (unsigned char)0x02};

                char power[3] = {(unsigned char)0xff, (unsigned char)0x07};

                if (serialBuf.indexOf(light_front, 0) == 0) {

                        emit sendLightMode(1);
                }
                else if (serialBuf.indexOf(light_back, 0) == 0) {

                        emit sendLightMode(2);
                }
                else if (serialBuf.indexOf(power, 0) == 0) {

                        short a = serialBuf[2];
                        short b = serialBuf[3];
                        int value = a << 8 | b;
                        emit showLeftPowerSignal(value);
                }

#endif
                //  sleep(0.1);
                if (isConnected_) {
                        //            f = sendToServer(tmp);
                        //                   QByteArray serialBuf;
                        //                 serialBuf.append("test socket\n");
                        emit sendToServerSignal(serialBuf);
                        //                  LOG_INFO<<"*****************\n";
                        //   printf("send: %x\n", tmp);
                } else
                        ; // printf("not connected !\n");
        }
}

void CtrlComm::openComm() {
        serial_->setPortName("/dev/ttyAMA0");
        serial_->setBaudRate(9600);
        if (!serial_->open()) {
                LOG_ERROR << "open failed !\n\n";
                // TODO send signal tor xcar.cpp, then show the error
                return;
        }
}

// may never be called
void CtrlComm::closeComm() {
        serial_->clear();
        serial_->close();
}

// --------------- network ------------------

int CtrlComm::connectToServer(const char *ip, int port) {
        if (isConnected_)
                return 0;

        carSocket_->abort();
        carSocket_->connectToHost(ip, port);

        QVariant vvv(1);
        carSocket_->setSocketOption(QAbstractSocket::LowDelayOption, vvv);
        carSocket_->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 0);

        carSocket_->waitForConnected(timeOut * 1000);

        LOG_DEBUG << "connecting to server..." << endl;
}

int CtrlComm::sendToServer(QByteArray data) {

        carSocket_->write(data, 5);
        carSocket_->flush();
        carSocket_->waitForBytesWritten(timeOut);

        //   double time_Start = (double)clock(); //开始时间
        // usleep(100);
        // double time_Finish = (double)clock(); //结束时间
        // LOG_DEBUG<<"operate time: "<<(time_Finish-time_Start);

        //        LOG_DEBUG<<"send msg to server"<<data<<endl;
}

void CtrlComm::connectedSlot() {
        isConnected_ = true;

        LOG_DEBUG << "connect to server success !\n";
}

void CtrlComm::disconnectedSlot() {
        carSocket_->close();
        isConnected_ = false;

        LOG_DEBUG << "disconnect from server ... \n";
}

void CtrlComm::readyReadSlot() {
        //    while (carSocket_->bytesAvailable() < 1) {
        //        if (!carSocket_->waitForReadyRead(timeOut))
        //            return;
        //        }

        // 0724/21:32     QByteArray message = carSocket_->read(5);
        // carSocket_->waitForReadyRead(timeOut);

        // LOG_DEBUG<<"read msg from server"<<message<<endl;

        // 0724/21:32
        // socketArray_.append(message);
        socketArray_.append(carSocket_->readAll());
        // LOG_DEBUG<<"recv data: "<<socketArray_.toHex();

        // 0xFF 0xFF
        char spos[2] = {(unsigned char)0xff};
        char epos[2] = {(unsigned char)0xff};

        int startPos = socketArray_.indexOf(spos, 0);
        if (startPos == -1) {
                LOG_ERROR << "1 not find data\n ";
                socketArray_.clear();
                return;
        }

        int endPos = socketArray_.indexOf(epos, startPos + 1);
        if (endPos == -1) {
                LOG_ERROR << "2 not find data\n ";
                return;
        }

        if (endPos - startPos == 1) {
                // socketArray_.clear();
                socketArray_.remove(0, startPos + 1);
                LOG_ERROR << "find data 0XFFFF, return\n ";
                return;
        }

        QByteArray serialBuf = socketArray_.mid(startPos, endPos - startPos + 1);

        //  QByteArray t = serialBuf.toHex();
        // LOG_ERROR<<t<<"len = "<<socketArray_.size();
        LOG_ERROR << "len = " << socketArray_.size();
        socketArray_.clear();
        // socketArray_.remove(0, startPos + 5);

        short a = serialBuf[2];
        short b = serialBuf[3];

        int value = a << 8 | b;

        char speed1[3] = {(unsigned char)0xff, (unsigned char)0x05};
        char speed2[3] = {(unsigned char)0xff, (unsigned char)0x06};

        char power[3] = {(unsigned char)0xff, (unsigned char)0x08};

        char tmperature1[3] = {(unsigned char)0xff, (unsigned char)0x0a};
        char tmperature2[3] = {(unsigned char)0xff, (unsigned char)0x0b};

        char angle1[3] = {(unsigned char)0xff, (unsigned char)0x0c};
        char angle2[3] = {(unsigned char)0xff, (unsigned char)0x0d};

        if (serialBuf.indexOf(speed1, 0) == 0) {
                emit showSpeedSignal(value);
        }
        else if (serialBuf.indexOf(speed2, 0) == 0) {
                emit showSpeedSignal(0 - value);
        }
        else if (serialBuf.indexOf(power, 0) == 0) {
                emit showRightPowerSignal(value);
                serial_->writeData(serialBuf.data(), 5); // write back to serial port
        } else if (serialBuf.indexOf(tmperature1, 0) == 0 ||
                   serialBuf.indexOf(tmperature2, 0) == 0) {
                emit showTemperatureSignal(value);
        }
        else if (serialBuf.indexOf(angle1, 0) == 0) {
                emit showAngleSignal(value);
        } else if (serialBuf.indexOf(angle2, 0) == 0) {
                emit showAngleSignal(0 - value);
        }

        // void showDistanceSignal(float x);
        // void showLeftPowerSignal(float x);
}

void CtrlComm::errorSlot(QAbstractSocket::SocketError) {
        LOG_ERROR << "socket error !\n";

        disconnectedSlot();
        sleep(1);
        connectToServer("127.0.0.1", 2001);
}
