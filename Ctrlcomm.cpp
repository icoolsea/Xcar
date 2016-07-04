#include "CtrlComm.h"

CtrlComm::CtrlComm() : isConnected_(false)
{

}

// --------------- Serial Port ------------------

CtrlComm::~CtrlComm() {
        serialClose(serialPortFd_);
        delete carSocket_;
}

void CtrlComm::run() {

        openComm();

        carSocket_ = new QTcpSocket();
        connect(carSocket_, SIGNAL(connected()), this, SLOT(connectedSlot()));
        connect(carSocket_, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
        connect(carSocket_, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
        connect(carSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this,
                SLOT(errorSlot(QAbstractSocket::SocketError)));


        carSocket_->connectToHost("127.0.0.1", 2001);
        isConnected_ = true;


        while (true) {
                char tmp;
                memset(&tmp, '0x00', 1);
                tmp = serialGetchar (serialPortFd_) ;
                // printf("@: %x\n", tmp);

                int f;
                if (tmp != -1 && isConnected_) {
                        f = carSocket_->write(&tmp, 1);
                        carSocket_->waitForBytesWritten(1000);
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

void CtrlComm::connectedSlot()
{

        isConnected_ = true;
        printf("@@@@@@@@@@@@@@@@@@@@@@@  ok\n");
}

void CtrlComm::disconnectedSlot()
{
        isConnected_ = false;
        carSocket_->close();
}

void CtrlComm::readyReadSlot()
{
        QByteArray message = carSocket_->readAll();
        carSocket_->waitForReadyRead(1000);
        printf("%s\n", message.toStdString().c_str());
        //    QMessageBox::information(this, "show", message);

        //TODO
        //ui->recvEdit->append(message);
}

void CtrlComm::errorSlot(QAbstractSocket::SocketError)
{
        //TODO send to xcar
        //    QMessageBox::information(this, "show", carSocket_->errorString());
        disconnectedSlot();
}

