#include "portique.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include "QtDebug"

Portique::Portique()
{
    serial = new QSerialPort();
    serial->setPortName("ttyS0");
    if(!serial->setBaudRate(QSerialPort::Baud9600))
    {
        qDebug() << "Erreur lors de l'initialisation du baude rate à 9600";
        return;
    }

    if(!serial->setStopBits(QSerialPort::OneStop))
    {
        qDebug() << "Erreur lors de l'initialisation du stop bit à 1";
        return;
    }

    if(!serial->setDataBits(QSerialPort::Data8))
    {
        qDebug() << "Erreur lors de l'initialisation à 8 bits";
        return;
    }

    if(!serial->setFlowControl(QSerialPort::NoFlowControl))
    {
        qDebug() << "Erreur lors de l'initialisation du flux de controle en non flux de controle";
    }

    if(serial->open(QIODevice::ReadOnly))
    {


        connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));
        qDebug() << serial->readAll();

    } else {
        qDebug() << "Erreur lors de la lecture !" << serial->error();
    }

}

void Portique::serialReceived()
{
    QByteArray data = serial->readAll();
    qDebug() << data;


}
