#include "arduino.h"
#include <QDebug>

Arduino::Arduino(QObject *parent)
    : QObject(parent),
      serial(new QSerialPort(this)),
      arduino_port_name(""),
      arduino_is_available(false)
{
    connect(serial, &QSerialPort::readyRead, this, &Arduino::onReadyRead);
}

Arduino::~Arduino()
{
    close_arduino();
}

QStringList Arduino::available_ports()
{
    QStringList ports;
    const auto portList = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portList) {
        ports << info.portName();
    }
    return ports;
}

int Arduino::connect_arduino(const QString &portName, qint32 baudRate)
{
    if (serial->isOpen()) {
        serial->close();
    }

    arduino_port_name = "";
    arduino_is_available = false;

    // Si un port précis est spécifié
    if (!portName.isEmpty()) {
        arduino_port_name = portName;
        arduino_is_available = true;
    } else {
        // Recherche automatique par Vendor ID et Product ID
        const auto portList = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : portList) {
            if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
                if ((info.vendorIdentifier() == arduino_uno_vendor_id && info.productIdentifier() == arduino_uno_product_id) ||
                    (info.vendorIdentifier() == arduino_ch340_vendor_id && info.productIdentifier() == arduino_ch340_product_id)) {
                    arduino_port_name = info.portName();
                    arduino_is_available = true;
                    break;
                }
            }
        }

        // Si non trouvé par ID, tenter le premier port disponible (ex: com0com / pont Wokwi)
        if (!arduino_is_available && !portList.isEmpty()) {
            arduino_port_name = portList.first().portName();
            arduino_is_available = true;
        }
    }

    if (!arduino_is_available || arduino_port_name.isEmpty()) {
        emit connectionStatusChanged(false, "Aucun port série Arduino détecté.");
        return -1;
    }

    serial->setPortName(arduino_port_name);
    if (!serial->open(QSerialPort::ReadWrite)) {
        emit connectionStatusChanged(false, QString("Impossible d'ouvrir le port %1 : %2")
                                                .arg(arduino_port_name, serial->errorString()));
        return 1;
    }

    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    emit connectionStatusChanged(true, QString("Connecté avec succès sur %1 (%2 bauds)")
                                           .arg(arduino_port_name).arg(baudRate));
    return 0;
}

int Arduino::close_arduino()
{
    if (serial && serial->isOpen()) {
        serial->close();
        emit connectionStatusChanged(false, "Liaison série déconnectée.");
        return 0;
    }
    return 1;
}

int Arduino::write_to_arduino(const QByteArray &d)
{
    if (serial && serial->isWritable()) {
        qint64 bytesWritten = serial->write(d);
        serial->flush();
        return (bytesWritten == d.size()) ? 0 : 1;
    }
    return -1;
}

QByteArray Arduino::read_from_arduino()
{
    if (serial && serial->isReadable()) {
        return serial->readAll();
    }
    return QByteArray();
}

void Arduino::onReadyRead()
{
    readBuffer.append(serial->readAll());

    // Découpage par ligne (\n ou \r\n)
    while (readBuffer.contains('\n')) {
        int idx = readBuffer.indexOf('\n');
        QByteArray lineBytes = readBuffer.left(idx);
        readBuffer.remove(0, idx + 1);

        QString line = QString::fromUtf8(lineBytes).trimmed();
        if (!line.isEmpty()) {
            emit lineReceived(line);
        }
    }
}

