#include "server.h"
#include <QDebug>

Server *Server::instance = nullptr;

Server *Server::getInstance()
{
    if (!instance)
        instance = new Server();
    return instance;
}

Server::Server() {}

void Server::init()
{
    socket = new QUdpSocket(this);
    socket->bind(AppConstants::UDP_PORT);

    /* Connexion du signal readyRead au slot de traitement des datagrammes */
    connect(socket, &QUdpSocket::readyRead, this, &Server::lireDatagrammesEntrants);

    qDebug() << "Serveur UDP en écoute sur le port :" << AppConstants::UDP_PORT;
}

QUdpSocket *Server::getClient() const  { return socket; }
QHostAddress Server::getLecteurSender() const { return lecteurSender; }
int Server::getLecteurPort() const { return lecteurPort; }

void Server::send(const QByteArray &data)
{
    if (socket && socket->isValid())
        socket->writeDatagram(data, lecteurSender, lecteurPort);
}

/*
 * Traite tous les datagrammes UDP en attente dans le buffer du socket.
 * Le comportement dépend du mode actuel de la course (DEFAULT / RAZ / DATA).
 */
void Server::lireDatagrammesEntrants()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray donnees;
        donnees.resize(socket->pendingDatagramSize());

        QHostAddress emetteur;
        quint16 portEmetteur;
        socket->readDatagram(donnees.data(), donnees.size(), &emetteur, &portEmetteur);

        /* Ignorer les datagrammes si aucune course n'est sélectionnée */
        if (!RaceManager::getInstance()->isRaceSelected())
            continue;

        const int mode = RaceManager::getInstance()->getMode();

        if (mode == RaceManager::DEFAULT) {
            /* Premier contact avec le lecteur : mémoriser son adresse et lui demander
             * de passer en mode RAZ (réinitialisation / attribution des badges) */
            lecteurSender = emetteur;
            lecteurPort   = portEmetteur;

            qDebug() << "[DEFAULT] Lecteur connecté depuis" << emetteur.toString() << ":" << portEmetteur;

            socket->writeDatagram(QByteArray("R"), lecteurSender, lecteurPort);
            RaceManager::getInstance()->setMode(RaceManager::RAZ);

        } else if (mode == RaceManager::RAZ) {
            /* Mode RAZ : le lecteur envoie l'identifiant du badge RFID du coureur.
             * On filtre :
             *   - les paquets "coucou" (signal de présence du lecteur)
             *   - les trames trop courtes ou trop longues (l'ID badge fait 7 ou 8 caractères) */
            const bool estHeartbeat = donnees.contains("coucou");
            const int  longueur     = donnees.length();
            const bool longueurValide = (longueur >= AppConstants::CARD_ID_MIN_LEN &&
                                         longueur <= AppConstants::CARD_ID_MAX_LEN);

            if (!estHeartbeat && longueurValide) {
                qDebug() << "[RAZ] Badge scanné :" << donnees;
                emit getCardId(QString::fromUtf8(donnees));
            }

        } else if (mode == RaceManager::DATA) {
            /* Mode DATA : le lecteur envoie les résultats de passage du coureur.
             * La trame doit contenir au moins un ID de carte et une balise (séparés par des virgules). */
            if (donnees.split(',').count() > 2)
                traiterTrameData(donnees);
            else
                qWarning() << "[DATA] Trame ignorée (format invalide) :" << donnees;
        }
    }
}

/*
 * Analyse une trame DATA complète reçue du lecteur.
 *
 * Format attendu : <IdCarte>,<Balise1>,<Balise2>,...
 *   Chaque balise : <NomBalise>_<GPS1>_<GPS2>_<Points>
 *   Exemple : c6bf9321,C1-B1_4931.3112_04263.1056_5,C1-B2_4934.3112_02363.2356_1
 */
void Server::traiterTrameData(const QByteArray &trame)
{
    QList<QByteArray> elements = trame.split(',');

    const QString carteId   = QString::fromUtf8(elements.at(0));
    int   pointsTotal = 0;
    int   nbBalises   = 0;

    /* Parcourir chaque balise (à partir de l'index 1) */
    for (int i = 1; i < elements.size(); ++i) {
        QList<QByteArray> champsBalise = elements.at(i).split('_');

        /* Format valide : NomBalise_GPS1_GPS2_Points → 4 champs minimum */
        if (champsBalise.size() < 4) {
            qWarning() << "[DATA] Balise malformée, ignorée :" << elements.at(i);
            continue;
        }

        const int points = champsBalise.at(3).trimmed().toInt();
        pointsTotal += points;
        nbBalises++;
    }

    qDebug() << "==== RÉSULTAT LECTEUR ====";
    qDebug() << "Mode        : DATA";
    qDebug() << "Course ID   :" << RaceManager::getInstance()->getRaceId();
    qDebug() << "Badge ID    :" << carteId;
    qDebug() << "Points      :" << pointsTotal;
    qDebug() << "Nb balises  :" << nbBalises;
    qDebug() << "=========================";

    emit getResultDataParticipant(carteId, pointsTotal, nbBalises);
}
