#ifndef CTRLCOMM_
#define CTRLCOMM_


#include <QDebug>
#include <QThread>

//#include "SerialThread.h"


#include <wiringSerial.h>
#include <wiringPi.h>


class CtrlComm : public QThread
{
Q_OBJECT
signals:
    void sSignal(char value);

protected:
    void run() {

        openComm();

        while (true) {

                char tmp;
                memset(&tmp, '0xff', 1);
                tmp = serialGetchar (fd) ;
                printf("@: %x\n", tmp);

                //if (tmp != -1)
                 //   serialPutchar(fd, tmp);
//                    emit sSignal(tmp);
        }

        closeComm();

    }

public:
    explicit CtrlComm()
    {
            //        m_spcomm = new SerialThread();

    }

    ~CtrlComm() {
            //        delete m_spcomm;
    }

    void openComm()
    {

            if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
//            if ((fd = serialOpen ("/dev/ttyS0", 115200)) < 0)
            {
                printf("open failed !\n\n");
                    return  ;
            }



            //        if(m_spcomm->isOpen())
            //                return;
            //            m_spcomm->setBaudRate(115200);
            //            m_spcomm->setPortName("/dev/ttyS0");
            //            if(m_spcomm->open())
            //                qDebug() << "打开串口成功";
    }

    void closeComm() {
            //        if(m_spcomm->isOpen())
            //                m_spcomm->close();
            //                qDebug() << "关闭串口成功";
    }

private:
    //    SerialThread *m_spcomm;
    int fd;

};


#endif // CTRL

