#ifndef CTRLCOMM_
#define CTRLCOMM_

#include <QByteArray>
#include <QThread>
#include <QtNetwork/QTcpSocket>

#include "SerialThread.h"

class CtrlComm : public QThread {
    Q_OBJECT
signals:
    void sSignal(char value);
    void sendToServerSignal(QByteArray);

    void showSpeedSignal(float value);
    void showAngleSignal(float x);
    void showTemperatureSignal(float x);

    void showLeftPowerSignal(float x);
    void showRightPowerSignal(float x);

    void sendLightMode(int mode);

protected:
    void run();

public:
    explicit CtrlComm();
    ~CtrlComm();

    void openComm();

    // may never be called
    void closeComm();

private slots:
    void connectedSlot();
    void disconnectedSlot();
    void readyReadSlot();
    void errorSlot(QAbstractSocket::SocketError);

    int sendToServer(QByteArray data);

private:
    int connectToServer(const char *ip, int port);

    QTcpSocket *carSocket_;
    bool isConnected_;

    SerialThread *serial_;
    QByteArray serialArray_;
    QByteArray socketArray_;
};

#endif // CTRL
