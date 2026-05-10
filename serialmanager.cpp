#include "serialmanager.h"
#include <QDebug>

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
    , m_serial(new QSerialPort(this))
{
    connect(m_serial, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialManager::onErrorOccurred);
}

SerialManager::~SerialManager()
{
    closePort();
}

QStringList SerialManager::availablePorts() const
{
    QStringList ports;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ports << info.portName();
    return ports;
}

bool SerialManager::openPort(const QString &portName, qint32 baudRate,
                             QSerialPort::DataBits dataBits,
                             QSerialPort::StopBits stopBits,
                             QSerialPort::Parity parity,
                             QSerialPort::FlowControl flowControl)
{
    if (m_serial->isOpen())
        m_serial->close();

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(dataBits);
    m_serial->setStopBits(stopBits);
    m_serial->setParity(parity);
    m_serial->setFlowControl(flowControl);

    if (m_serial->open(QIODevice::ReadWrite)) {
        emit portOpened();
        return true;
    } else {
        emit portError(m_serial->errorString());
        return false;
    }
}

void SerialManager::closePort()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        emit portClosed();
    }
}

bool SerialManager::isPortOpen() const
{
    return m_serial->isOpen();
}

void SerialManager::sendData(const QByteArray &data)
{
    if (!m_serial->isOpen())
        return;

    QMutexLocker locker(&m_mutex);
    m_serial->write(data);
    m_serial->waitForBytesWritten(100);
    emit dataSent(data);
}

void SerialManager::sendAscii(const QString &text)
{
    sendData(text.toUtf8());
}

void SerialManager::sendHex(const QString &hexStr)
{
    QByteArray data = hexStringToBytes(hexStr);
    if (!data.isEmpty())
        sendData(data);
}

QByteArray SerialManager::hexStringToBytes(const QString &hex)
{
    QString cleanHex = hex.simplified().remove(' ');
    if (cleanHex.length() % 2 != 0)
        return QByteArray();

    QByteArray result;
    for (int i = 0; i < cleanHex.length(); i += 2) {
        bool ok;
        uint byte = cleanHex.mid(i, 2).toUInt(&ok, 16);
        if (!ok)
            return QByteArray();
        result.append(static_cast<char>(byte));
    }
    return result;
}

QString SerialManager::bytesToHexString(const QByteArray &data)
{
    return data.toHex(' ').toUpper();
}

QString SerialManager::bytesToAsciiString(const QByteArray &data)
{
    return QString::fromUtf8(data);
}

void SerialManager::onReadyRead()
{
    QByteArray data = m_serial->readAll();
    if (!data.isEmpty())
        emit dataReceived(data);
}

void SerialManager::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
        return;
    emit portError(m_serial->errorString());
}
