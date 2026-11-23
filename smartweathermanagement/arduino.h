#ifndef ARDUINO_H
#define ARDUINO_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>

class Arduino : public QObject
{
    Q_OBJECT
public:
    explicit Arduino(QObject *parent = nullptr);
    ~Arduino();

    int connect_arduino(const QString &portName = QString(), qint32 baudRate = QSerialPort::Baud9600);
    int close_arduino();
    int write_to_arduino(const QByteArray &d);
    QByteArray read_from_arduino();

    QSerialPort* getserial() { return serial; }
    QString getarduino_port_name() const { return arduino_port_name; }
    bool is_available() const { return arduino_is_available; }
    bool is_connected() const { return serial && serial->isOpen(); }

    static QStringList available_ports();

signals:
    void lineReceived(const QString &line);
    void connectionStatusChanged(bool connected, const QString &message);

private slots:
    void onReadyRead();

private:
    QSerialPort *serial;
    static const quint16 arduino_uno_vendor_id = 9025;   // 0x2341 (Arduino Uno officiel)
    static const quint16 arduino_uno_product_id = 67;    // 0x0043 (Arduino Uno officiel)
    static const quint16 arduino_ch340_vendor_id = 6790; // 0x1A86 (Arduino CH340 clones / Wokwi bridge)
    static const quint16 arduino_ch340_product_id = 29987;

    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray readBuffer;
};

#endif // ARDUINO_H

