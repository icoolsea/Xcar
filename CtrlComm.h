#ifndef CTRLCOMM_
#define CTRLCOMM_


#include <QDebug>
#include <QThread>
#include <QtNetwork/QTcpSocket>


//#include "SerialThread.h"

#include <wiringSerial.h>
#include <wiringPi.h>


class CtrlComm : public QThread
{
Q_OBJECT
signals:
    void sSignal(char value);
    void sendToServerSignal(char value);

protected:
    void run();


public:
    explicit CtrlComm();
    ~CtrlComm();

    void openComm();

    //may never be called
    void closeComm();


private slots:
    void connectedSlot();
    void disconnectedSlot();
    void readyReadSlot();
    void errorSlot(QAbstractSocket::SocketError);

    int sendToServer(char data);


private:
    int connectToServer(const char * ip, int port);


    int serialPortFd_;

    QTcpSocket *carSocket_;
    bool isConnected_;

};


#endif // CTRL

