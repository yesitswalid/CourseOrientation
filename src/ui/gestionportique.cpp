#include "ui_gestionportique.h"
#include "gestionportique.h"
#include "racemanager.h"
#include "server.h"

GestionPortique::GestionPortique()
{
    ui.setupUi(this);
    ui.modeLabel->setText("Mode : " + getModeToString());
}

GestionPortique::~GestionPortique() {}

/* Retourne le nom lisible du mode actuel pour l'affichage dans l'interface */
QString GestionPortique::getModeToString()
{
    return (RaceManager::getInstance()->getMode() == RaceManager::DATA) ? "DATA" : "RAZ";
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Bouton RAZ : demande au lecteur de passer en mode attribution de badges.
 *  Envoie "R" via UDP et met à jour le mode local.
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::on_razButton_clicked()
{
    if (!Server::getInstance()->getClient()->isValid()) {
        QMessageBox::information(this, "Portique",
            "Aucun lecteur connecté. Vérifiez la communication UDP.");
        return;
    }

    Server::getInstance()->send(QByteArray("R"));
    RaceManager::getInstance()->setMode(RaceManager::RAZ);
    ui.modeLabel->setText("Mode : " + getModeToString());
    QMessageBox::information(this, "Portique", "Le lecteur est maintenant en mode RAZ.");
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Bouton DATA : demande au lecteur de passer en mode collecte de données.
 *  Envoie "D" via UDP et met à jour le mode local.
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::on_dataButton_clicked()
{
    if (!Server::getInstance()->getClient()->isValid()) {
        QMessageBox::information(this, "Portique",
            "Aucun lecteur connecté. Vérifiez la communication UDP.");
        return;
    }

    Server::getInstance()->send(QByteArray("D"));
    RaceManager::getInstance()->setMode(RaceManager::DATA);
    ui.modeLabel->setText("Mode : " + getModeToString());
    QMessageBox::information(this, "Portique", "Le lecteur est maintenant en mode DATA.");
}
