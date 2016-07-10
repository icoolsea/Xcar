#include "CtrlComm.h"
#include "Log.h"

#include <QtWidgets>
#include <QSettings>
#include <QByteArray>

static const int timeOut = 100;


CtrlComm::CtrlComm() : isConnected_(false)
{
    serial_ = new SerialThread;

}


CtrlComm::~CtrlComm() {
    closeComm();
    delete serial_;
    delete carSocket_;
}

void CtrlComm::run() {


        openComm();

        carSocket_ = new QTcpSocket();

        connect(carSocket_, SIGNAL(connected()), this, SLOT(connectedSlot()), Qt::DirectConnection);
        connect(carSocket_, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()), Qt::DirectConnection);
        connect(carSocket_, SIGNAL(readyRead()), this, SLOT(readyReadSlot()), Qt::DirectConnection);
        connect(carSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this,
                SLOT(errorSlot(QAbstractSocket::SocketError)), Qt::DirectConnection);

        connect(this, SIGNAL(sendToServerSignal(QByteArray)), this,
                SLOT(sendToServer(QByteArray)), Qt::DirectConnection);


        QSettings *configIniRead = new QSettings("xcar.ini", QSettings::IniFormat);
        //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
        QString ipResult = configIniRead->value("/server/ip").toString();
        QString portResult = configIniRead->value("/server/port").toString();
        //打印得到的结果
        qDebug() << ipResult;
        qDebug() << portResult;
        delete configIniRead;


        connectToServer(ipResult.toStdString().c_str(), portResult.toShort());

        while (true) {

#if 1
                if (!serial_->isOpen())
                {
                    sleep(1);
                    LOG_ERROR<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
                    continue;
                }
                unsigned char tmp[5];
                memset(&tmp, 0x00, 5);
                int count = serial_->readData((char *)tmp, 5);
                if (count <= 0)
                {
                        continue;
                }


                //  int count = serialArray_.append(tmp);
                memcpy(serialArray_.data(), tmp, count);

                //0xFF 0xFF
                char spos[3] = {(unsigned char)0xff};
                char epos[3] = {(unsigned char)0xff};

                int startPos = serialArray_.indexOf(spos, 0);
                if (startPos == -1)
                {
                        serialArray_.clear();
                        continue;
                }

                int endPos = serialArray_.indexOf(epos, startPos);
                if (endPos == -1)
                {
                        continue;
                }

                QByteArray serialBuf = serialArray_.mid(startPos, endPos-startPos+2);

                serialArray_.clear();

#endif
                //  sleep(0.1);
                if (isConnected_ ) {
                        //            f = sendToServer(tmp);
                        //                   QByteArray serialBuf;
                        //                 serialBuf.append("test socket\n");
                        emit sendToServerSignal(serialBuf);
                        //                  LOG_INFO<<"*****************\n";
                        //   printf("send: %x\n", tmp);
                }else
                        ;// printf("not connected !\n");
        }
}

void CtrlComm::openComm()
{
        serial_->setPortName("/dev/ttyAMA0");
        serial_->setBaudRate(9600);
        if (!serial_->open())
        {
                LOG_ERROR<<"open failed !\n\n";
                //TODO send signal tor xcar.cpp, then show the error
                return  ;
        }
}

//may never be called
void CtrlComm::closeComm() {
        serial_->clear();
        serial_->close();
}


// --------------- network ------------------

int CtrlComm::connectToServer(const char * ip, int port)
{
        if (isConnected_)
                return 0;

        carSocket_->abort();
        carSocket_->connectToHost(ip, port);
        carSocket_->waitForConnected(timeOut);

        LOG_DEBUG<<"connecting to server..."<<endl;

}

int CtrlComm::sendToServer(QByteArray data)
{
        //carSocket_->write(&tmp, 1);
        carSocket_->write(data, 5);
        carSocket_->waitForBytesWritten(timeOut);

        //        LOG_DEBUG<<"send msg to server"<<data<<endl;

}

void CtrlComm::connectedSlot()
{
        isConnected_ = true;

        LOG_DEBUG<<"connect to server success !\n";
}

void CtrlComm::disconnectedSlot()
{
        carSocket_->close();
        isConnected_ = false;

        LOG_DEBUG<<"disconnect from server ... \n";
}

void CtrlComm::readyReadSlot()
{
        //    while (carSocket_->bytesAvailable() < 1) {
        //        if (!carSocket_->waitForReadyRead(timeOut))
        //            return;
        //        }

        QByteArray message = carSocket_->read(1024);
        // carSocket_->waitForReadyRead(timeOut);

        LOG_DEBUG<<"read msg from server"<<message<<endl;
}

void CtrlComm::errorSlot(QAbstractSocket::SocketError)
{
        LOG_ERROR<<"socket error !\n";

        disconnectedSlot();
        sleep(1);
        connectToServer("127.0.0.1", 2001);
}

