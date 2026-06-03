#ifndef PORTIQUE_H
#define PORTIQUE_H

#include <QSerialPort>
#include <QObject>
#include <QByteArray>


class Portique : public QObject
{
    Q_OBJECT

public:
    Portique();
private:
    QSerialPort *serial;
private slots:
      void serialReceived();
      void doWork();
signals:
      void workFinished();
      void getData(QByteArray);
};

#endif // PORTIQUE_H
