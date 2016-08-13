#include "Camclient.h"
#include <QFile>

CamClient::CamClient(QObject *parent) : QObject(parent) {
        showMode = false;
        connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(readImage()));
}

void CamClient::connectToHost(QHostAddress addr, quint16 port) {
        tcpSocket.connectToHost(addr, port);
}

void CamClient::requestImage() {
        QByteArray imgReqArray;
        imgReqArray.append("GET /?action=stream HTTP/1.1\r\n\r\n");

        tcpSocket.write(imgReqArray);
}

void CamClient::readImage() {
        imageArray.append(tcpSocket.readAll());

        // FFD8 FFD9
        char spos[3] = {(unsigned char)0xff, (unsigned char)0xd8};
        char epos[3] = {(unsigned char)0xff, (unsigned char)0xd9};

        int startPos = imageArray.indexOf(spos, 0);
        if (startPos == -1) {
                imageArray.clear();
                return;
        }

        int endPos = imageArray.indexOf(epos, startPos);
        if (endPos == -1) {
                return;
        }

        if (isShow()) {
                QByteArray imageBuf = imageArray.mid(startPos, endPos - startPos + 2);
                //image.loadFromData(imageBuf, "JPEG");
                //emit newImageReady(image);
                emit newImageReady_new(imageBuf);
        }

        imageArray.clear();
}

void CamClient::enableShow() { showMode = true; }

void CamClient::disableShow() { showMode = false; }

bool CamClient::isShow() { return showMode; }
