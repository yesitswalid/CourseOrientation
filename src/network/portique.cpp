#include "portique.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

/* Le nom du port série dépend de la plateforme — défini dans le .pro */
#ifndef SERIAL_PORT_DEFAULT
#  ifdef Q_OS_WIN
#    define SERIAL_PORT_DEFAULT "COM3"
#  else
#    define SERIAL_PORT_DEFAULT "/dev/ttyUSB0"
#  endif
#endif

Portique::Portique()
{
    serial = new QSerialPort(this);
    serial->setPortName(QString::fromLatin1(SERIAL_PORT_DEFAULT));

    if (!serial->setBaudRate(QSerialPort::Baud9600))
        qWarning() << "[Portique] Impossible de configurer le débit à 9600 bauds";
    if (!serial->setStopBits(QSerialPort::OneStop))
        qWarning() << "[Portique] Impossible de configurer le bit d'arrêt à 1";
    if (!serial->setDataBits(QSerialPort::Data8))
        qWarning() << "[Portique] Impossible de configurer le format de données à 8 bits";
    if (!serial->setFlowControl(QSerialPort::NoFlowControl))
        qWarning() << "[Portique] Impossible de désactiver le contrôle de flux";

    if (!serial->open(QIODevice::ReadOnly))
        qWarning() << "[Portique] Erreur d'ouverture du port série :" << serial->errorString();
}

void Portique::doWork()
{
    while (true) {
        QByteArray data = serial->readAll();
        if (!data.isEmpty()) {
            emit getData(data);
            break;
        }
    }
    serial->flush();
    emit workFinished();
}

void Portique::serialReceived()
{
    QByteArray data = serial->readAll();
    if (!data.isEmpty())
        qDebug() << "[Portique] Données série reçues :" << data;
}
