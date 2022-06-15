#include <QDebug>
#include <QThread>
#include <QUdpSocket>
#include "racemanager.h"
#include "server.h"


Server* Server::instance = 0;

Server* Server::getInstance()
{
        if (instance == 0)
        {
            instance = new Server();
        }
        return instance;
}

Server::Server()
{

}


void Server::init()
{
        socket = new QUdpSocket();
        socket->bind(PORT);
        socket->connect(socket, SIGNAL(readyRead()),this,SLOT(readPendingDiagrams()));
        qDebug() << "Attente de données depuis le port: " << PORT << " ... \n";
}


QUdpSocket* Server::getClient()
{
    return socket;
}


void Server::send(QByteArray data)
{
    socket->writeDatagram(data, lecteurSender, lecteurPort);
}

QHostAddress Server::getLecteurSender()
{
    return this->lecteurSender;
}

int Server::getLecteurPort()
{
    return this->lecteurPort;
}


void Server::readPendingDiagrams()
{

        QByteArray donneesCarte;

        donneesCarte.resize(socket->pendingDatagramSize());
        QHostAddress sender;

        quint16 senderPort;



        socket->readDatagram(donneesCarte.data(), donneesCarte.size(),
                             &sender, &senderPort);


        lecteurSender = sender;
        lecteurPort = senderPort;

        if(RaceManager::getInstance()->isRaceSelected())
        {

        //socket->writeDatagram(QByteArray("aa"), lecteurSender, lecteurPort);

        /*

        if(RaceManager::getInstance()->getMode() != MODE_SERVER)  //changement de mode est avertie le client du changement
        {
            MODE_SERVER = RaceManager::getInstance()->getMode();

            switch (MODE_SERVER)
            {
               case SERVER_RAZ:
                  socket->writeDatagram(QByteArray("R"), lecteurSender, lecteurPort);
                break;
               case SERVER_DATA:
                socket->writeDatagram(QByteArray("D"), lecteurSender, lecteurPort);
                break;
            }

        }
        */



        if(RaceManager::getInstance()->getMode() == RaceManager::DEFAULT)
        {

            qDebug() << "Client from (DEFAULT): " << sender.toString();
            qDebug() << "Client port: " << senderPort;
            qDebug() << "donneesCarte: " << donneesCarte;

            lecteurSender = sender;
            lecteurPort = senderPort;

            socket->writeDatagram(QByteArray("R"), lecteurSender, lecteurPort);
            RaceManager::getInstance()->setMode(RaceManager::RAZ);

        } else if(RaceManager::getInstance()->getMode() == RaceManager::RAZ) {
                qDebug() << "Client from (RAZ): " << sender.toString();
                qDebug() << "Client port: " << senderPort;
                qDebug() << "donneesCarte: " << donneesCarte;

                if(!donneesCarte.contains("coucou") and (donneesCarte.length() > 6 && donneesCarte.length() < 9)) //evite le crash et récupére la données souhaité entre 7 et 8 = id de la carte
                {
                    emit getCardId(donneesCarte);
                }

        } else if(RaceManager::getInstance()->getMode() == RaceManager::DATA)
        {

            qDebug() << "Client from (DATA): " << sender.toString();
            qDebug() << "Client port: " << senderPort;
            qDebug() << "donneesCarte: " << donneesCarte;


            if(donneesCarte.split(',').count() > 2)
            {

                qDebug() << "Bonne TRAME: " << donneesCarte;

                //c6bf9321,C1-B1_4931.3112_04263.1056_5,C1-B2_4934.3112_02363.2356_1

                QList <QByteArray> trames = donneesCarte.split(',');


                QString carteId = trames[0];

                int nbBalise = 0;
                int pointsTotal = 0;
                for(int i = 1; i < trames.size(); i++)
                {

                    QList <QByteArray> balises = trames[i].split('_');
                    int points = balises[3].toInt();
                    pointsTotal += points;
                    nbBalise++;
                }

                qDebug() << "======= LECTEUR ==========";
                qDebug() << "Mode: DATA";
                qDebug() << "Course ID: " << RaceManager::getInstance()->getRaceId();
                qDebug() << "Carte ID: " << carteId; //participantId
                qDebug() << "Points Total: " << pointsTotal;
                qDebug() << "nombre de balise : " << nbBalise;
                qDebug() << "==========================";


                emit getResultDataParticipant(carteId, pointsTotal, nbBalise);

            }
        }
    }
}
