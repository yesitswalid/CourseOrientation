#ifndef SERVER_H
#define SERVER_H

#include <QDebug>
#include <QThread>
#include <QUdpSocket>
#include "racemanager.h"

#define PORT 1234


class Server : public QObject
{
    Q_OBJECT

protected:
        static Server *instance;

private:
    QUdpSocket *socket;
    QHostAddress lecteurSender;
    int lecteurPort;
public:
        Server();
        static Server* getInstance();
        void init();
        QUdpSocket* getClient();
        QHostAddress getLecteurSender();
        int getLecteurPort();
        void send(QByteArray data);

signals:
      void getCardId(QString);
      void getResultDataParticipant(QString, int, int);


public slots:
    void readPendingDiagrams();

};
#endif // SERVER_H
