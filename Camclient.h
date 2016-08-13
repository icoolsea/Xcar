#ifndef CAMCLIENT_H
#define CAMCLIENT_H

#include <QByteArray>
#include <QHostAddress>
#include <QImage>
#include <QObject>
#include <QTcpSocket>

class CamClient : public QObject {
        Q_OBJECT
public:
        explicit CamClient(QObject *parent = 0);

        void connectToHost(QHostAddress addr, quint16 port);
        void requestImage(void);
        void enableShow();
        void disableShow();

private:
        bool isShow();

        QTcpSocket tcpSocket;
        QByteArray imageArray;
        QImage image;

        bool showMode;

signals:
        void newImageReady(QImage img);
        void newImageReady_new(QByteArray img);

public slots:
        void readImage();
};

#endif // CAMCLIENT_H
