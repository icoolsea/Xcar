#include "Log.h"
#include "SerialThread.h"

static QSerialPort::BaudRate getBaudRate(int baudRate)

{
        switch (baudRate) {
        case 1200:
                return QSerialPort::Baud1200;
        case 2400:
                return QSerialPort::Baud2400;
        case 4800:
                return QSerialPort::Baud4800;
        case 9600:
                return QSerialPort::Baud9600;
        case 19200:
                return QSerialPort::Baud19200;
        case 38400:
                return QSerialPort::Baud38400;
        case 57600:
                return QSerialPort::Baud57600;
        case 115200:
                return QSerialPort::Baud115200;
        default:
                return QSerialPort::UnknownBaud;
        }
}

bool SerialThread::isOpen() const { return isOpen_; }

void SerialThread::setPortName(const QString &name) { m_portName = name; }

QString SerialThread::portName() const { return m_portName; }

void SerialThread::setBaudRate(int baudRate) { m_baudRate = baudRate; }

bool SerialThread::open() {
        if (m_serialPort->isOpen()) {
                return true;
        }

        m_serialPort->setPortName(m_portName);
        m_serialPort->setBaudRate(getBaudRate(m_baudRate));
        m_serialPort->setParity(QSerialPort::NoParity);
        m_serialPort->setDataBits(QSerialPort::Data8);
        m_serialPort->setStopBits(QSerialPort::OneStop);
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        m_serialPort->setReadBufferSize(1024);

        if (m_serialPort->open(QSerialPort::ReadWrite)) {
                isOpen_ = true;
                return true;
        }

        return false;
}

void SerialThread::close() {
        if (m_serialPort->isOpen()) {
                m_serialPort->close();
        }
}

bool SerialThread::clear() {
        if (m_serialPort->isOpen()) {
                m_serialPort->clear();
                this->close();
                return this->open();
        }

        return false;
}

int SerialThread::readData(char *buffer, int count, int timeout) {
        int len = 0;

        forever {
                int n = m_serialPort->read(&buffer[len], count - len);
                if (n == -1) {
                        return -1;
                } else if (n == 0 && !m_serialPort->waitForReadyRead(timeout)) {
                        return -2;
                } else {
                        len += n;
                        if (count == len)
                                break;
                }
        }

        return len;
}

int SerialThread::writeData(char *data, int size) {
        int len = 0;

        forever {
                int n = m_serialPort->write(&data[len], size - len);
                if (n == -1) {
                        return -1;
                }

                else {
                        len += n;
                        if (size == len)
                                break;
                }
        }

        return len;
}

SerialThread::SerialThread() {
        isOpen_ = false;
        m_serialPort = new QSerialPort();
        m_baudRate = 9600;
        m_portName = "";
}

SerialThread::~SerialThread() {
        isOpen_ = false;
        delete m_serialPort;
}
