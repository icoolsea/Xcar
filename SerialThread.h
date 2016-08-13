#ifndef SERIAL
#define SERIAL

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialThread:QObject
{
        Q_OBJECT
signals:

protected:

public:
        explicit SerialThread();
        ~SerialThread();

        bool isOpen() const;

        void setPortName(const QString &name);

        QString portName() const;

        void setBaudRate(int baudRate);

        int baudRate() const;

        virtual bool open();

        virtual void close();

        virtual bool clear();

        int readData(char *buffer, int count, int timeout = 100);

        int writeData(char *data, int size);

        int write(char ch);

private:

        bool isOpen_;
        QString m_portName;

        int m_baudRate;

        QSerialPort *m_serialPort;

};

#endif // SERIAL

