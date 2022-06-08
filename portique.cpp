#include "portique.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include "QtDebug"

Portique::Portique()
{
    serial = new QSerialPort();
    serial->setPortName("/dev/ttyUSB0");
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


        //connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));

    } else {
        qDebug() << "Erreur lors de la lecture !" << serial->error();
    }

}

void Portique::doWork()
{
    qDebug() << "doWork";
    bool isRead = true;
    while(isRead)
    {

       QByteArray data = serial->readAll();


       if(!data.isEmpty())
       {
           emit getData(data);
           isRead = false;
       }
    }

    //serial->close();
    emit workFinished();
    serial->flush();
}

void Portique::serialReceived()
{
    QByteArray data = serial->readAll();

    if(data.contains("TEST"))
    {
        qDebug() << data;
        serial->deleteLater();
    }
}
