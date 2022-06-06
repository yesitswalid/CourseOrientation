#ifndef PORTIQUE_H
#define PORTIQUE_H

#include <QSerialPort>
#include <QObject>


class Portique : public QObject
{
    Q_OBJECT

public:
    Portique();
private:
    QSerialPort *serial;
private slots:
      void serialReceived();
};

#endif // PORTIQUE_H
