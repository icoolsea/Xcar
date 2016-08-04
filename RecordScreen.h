#ifndef RECORDSCREEN
#define RECORDSCREEN

#include <QThread>

#define  FIFO_NAME  "/tmp/screen_pipe"

class RecordScreen : public QThread
{
Q_OBJECT
protected:
    void run();


public:
    explicit  RecordScreen();
    ~RecordScreen();




public slots:
    void stopRecord();


private:
    void recordScreen();
    bool isStoped();

    bool isStopRecord;
    int  fifo_fd;


};

#endif // RECORDSCREEN
