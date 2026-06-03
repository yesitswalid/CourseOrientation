#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include "racemanager.h"
#include "constants.h"

/*
 * Protocole UDP entre ce serveur et le lecteur de badges RFID
 *
 *  ÉTAT       LECTEUR ENVOIE              SERVEUR RÉPOND   PROCHAIN ÉTAT
 *  ─────────  ──────────────────────────  ───────────────  ─────────────
 *  DEFAULT    N'importe quel datagramme   "R" (reset)      RAZ
 *  RAZ        ID badge RFID (7-8 octets)  (rien)           RAZ
 *  DATA       Trame balises               (rien)           DATA
 *
 * Format trame DATA :
 *   <IdCarte>,<Balise1>,<Balise2>,...
 *   Exemple : c6bf9321,C1-B1_4931.3112_04263.1056_5,C1-B2_4934.3112_02363.2356_1
 *   Format balise : <NomBalise>_<GPS1>_<GPS2>_<Points>
 *
 * Changements de mode :
 *   DEFAULT → RAZ  : automatique à la première connexion du lecteur
 *   RAZ     → DATA : manuel via GestionPortique (bouton DATA)
 *   DATA    → RAZ  : manuel via GestionPortique (bouton RAZ)
 */

class Server : public QObject
{
    Q_OBJECT

public:
    static Server *getInstance();
    Server();

    void init();
    void send(const QByteArray &data);

    QUdpSocket  *getClient() const;
    QHostAddress getLecteurSender() const;
    int          getLecteurPort() const;

signals:
    /* Émis en mode RAZ quand un badge RFID est scanné */
    void getCardId(const QString &cardId);

    /* Émis en mode DATA quand un coureur a terminé son passage
     * carteId    : identifiant du badge RFID du coureur
     * points     : total des points cumulés sur toutes les balises
     * nbBalises  : nombre de balises validées */
    void getResultDataParticipant(const QString &carteId, int points, int nbBalises);

public slots:
    /* Lit et traite tous les datagrammes UDP en attente */
    void lireDatagrammesEntrants();

private:
    /* Traite une trame DATA reçue du lecteur et émet getResultDataParticipant */
    void traiterTrameData(const QByteArray &trame);

    static Server *instance;
    QUdpSocket   *socket      = nullptr;
    QHostAddress  lecteurSender;
    int           lecteurPort = 0;
};

#endif // SERVER_H
