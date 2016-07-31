#include "RecordScreen.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include <QPixmap>
#include <QByteArray>
#include <QScreen>
#include <QBuffer>
#include <QDial>
#include <QWidget>
#include <QMessageBox>

#include <QGuiApplication>

RecordScreen::RecordScreen()
{
    isStopRecord = false;
}

RecordScreen::~RecordScreen()
{

}

void RecordScreen::run()
{
    recordScreen();
}

//ffmpeg -threads 2 -y -r 30 -i %04d.jpg   ./output.mp4
void RecordScreen::recordScreen()
{
        FILE* file = NULL;
        int  fifo_fd;

        if(access(FIFO_NAME, F_OK) ==  -1)
        {
                fifo_fd = mkfifo(FIFO_NAME, 0777);
                if(fifo_fd < 0)
                {
                }
        }

        QScreen *screen = QGuiApplication::primaryScreen();

        while (!isStoped()) {
                QPixmap pix = screen->grabWindow(0);

                QByteArray ba;
                QBuffer buf(&ba);
                pix.save(&buf, "jpg");


                fifo_fd = open(FIFO_NAME, O_WRONLY);
                //                size_t num =write(fifo_fd, ba, ba.size());
                size_t num =write(fifo_fd, ba, ba.size());
                if (num == -1)
                {
                }

                usleep(40*1000);
        }

        ::close(fifo_fd);
}

void RecordScreen::stopRecord()
{
    isStopRecord = true;
}

bool RecordScreen::isStoped()
{
    return isStopRecord;
}

