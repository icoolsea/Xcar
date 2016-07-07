/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */


#include "Xcar.h"

#include <QApplication>

#include <QSerialPort>
#include <QSerialPortInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Player p;
    p.resize(640,480);
    p.playFile("/home/jon/qtest/dsa.ts");

    p.show();



    return a.exec();
}
