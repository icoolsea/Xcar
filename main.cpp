/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */


#include "Xcar.h"

#include <QApplication>

#include <QSerialPort>
#include <QSerialPortInfo>

int main(int argc, char *argv[])
{
 /*   QApplication a(argc, argv);
    Player p;
    p.resize(640,480);
    p.playFile("/home/jon/qtest/dsa.ts");

    p.show();
    */
    QCoreApplication a(argc, argv);

    // Example use QSerialPortInfo
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

        // Example use QSerialPort
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
            serial.close();
    }


    return a.exec();
}
