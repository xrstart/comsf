#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QMutex>
#include <QByteArray>
#include <QStringList>

class SerialManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    // Port management
    QStringList availablePorts() const;
    bool openPort(const QString &portName, qint32 baudRate,
                  QSerialPort::DataBits dataBits,
                  QSerialPort::StopBits stopBits,
                  QSerialPort::Parity parity,
                  QSerialPort::FlowControl flowControl);
    void closePort();
    bool isPortOpen() const;

    // Send data
    void sendData(const QByteArray &data);
    void sendAscii(const QString &text);
    void sendHex(const QString &hexStr);

    // Convert helpers
    static QByteArray hexStringToBytes(const QString &hex);
    static QString bytesToHexString(const QByteArray &data);
    static QString bytesToAsciiString(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void dataSent(const QByteArray &data);
    void portError(const QString &errorMsg);
    void portOpened();
    void portClosed();

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort *m_serial;
    QMutex m_mutex;
};

#endif // SERIALMANAGER_H
