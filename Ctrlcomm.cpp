#include "CtrlComm.h"
#include "Log.h"

#include <QtWidgets>

static const int timeOut = 1000;


CtrlComm::CtrlComm() : isConnected_(false)
{

}


CtrlComm::~CtrlComm() {
    serialClose(serialPortFd_);
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

    connect(this, SIGNAL(sendToServerSignal(char)), this,
            SLOT(sendToServer(char)), Qt::DirectConnection);

    connectToServer("127.0.0.1", 2001);

    while (true) {
        char tmp;
        memset(&tmp, '0x00', 1);
        //    tmp = serialGetchar (serialPortFd_) ;
        // printf("@: %x\n", tmp);
        sleep(10);

        tmp = '3';

        int f;
        if (tmp != -1 && isConnected_) {
//            f = sendToServer(tmp);
            emit sendToServerSignal(tmp);
            printf("send ...: %c !\n", tmp);
        }else
            ;// printf("not connected !\n");
        //if (tmp != -1)
        //   serialPutchar(fd, tmp);
        //                    emit sSignal(tmp);
    }

    closeComm();
}

void CtrlComm::openComm()
{
    if ((serialPortFd_ = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
    {
        printf("open failed !\n\n");
        //TODO send signal tor xcar.cpp, then show the error
        return  ;
    }
}

//may never be called
void CtrlComm::closeComm() {
    serialClose(serialPortFd_);
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

int CtrlComm::sendToServer(char data)
{
    char tmp = data;
    carSocket_->write(&tmp, 1);
    carSocket_->waitForBytesWritten(timeOut);

    LOG_DEBUG<<"send msg to server"<<data<<endl;

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
    while (carSocket_->bytesAvailable() < 1) {
        if (!carSocket_->waitForReadyRead(timeOut))
            return;
        }

    QByteArray message = carSocket_->read(1024);
  //  carSocket_->waitForReadyRead(timeOut);

    LOG_DEBUG<<"read msg from server"<<message<<endl;
}

void CtrlComm::errorSlot(QAbstractSocket::SocketError)
{
    LOG_ERROR<<"socket error !\n";

    disconnectedSlot();
    sleep(1);
    connectToServer("127.0.0.1", 2001);
}

