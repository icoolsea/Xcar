/* xcar
 * Copyright © 2016 wjj <icoolsea@hotmail.com>
 */

#include "Xcar.h"

#include <QApplication>

#include <QSerialPort>
#include <QSerialPortInfo>

int main(int argc, char *argv[]) {
        QApplication a(argc, argv);

        Player p;

        p.show();

        return a.exec();
}
