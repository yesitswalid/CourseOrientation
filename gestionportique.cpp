#include "ui_gestionportique.h"
#include "gestionportique.h"
#include "racemanager.h"
#include "server.h"


GestionPortique::GestionPortique()
{
    ui.setupUi(this);
    ui.modeLabel->setText("Mode: " + this->getModeToString());
}


GestionPortique::~GestionPortique()
{
}

//Envoyer R au client lorsqu'on clique sur le boutton data -> changer l'état de la course à RAZ
void GestionPortique::on_razButton_clicked()
{

    if(Server::getInstance()->getClient()->isValid())
    {
        Server::getInstance()->send(QByteArray("R"));
        RaceManager::getInstance()->setMode(RaceManager::RAZ);
        ui.modeLabel->setText("Mode: " + this->getModeToString());
        QMessageBox::information(this, "Course", "La course est en mode RAZ !");
    } else {
        QMessageBox::information(this, "Course", "Assurez-vous que le client communique avec le lecteur !");
    }
}

//Envoyer D au client lorsqu'on clique sur le boutton data -> changer l'état de la course à DATA
void GestionPortique::on_dataButton_clicked()
{

    if(Server::getInstance()->getClient()->isValid())
    {
        Server::getInstance()->send(QByteArray("D"));
        RaceManager::getInstance()->setMode(RaceManager::DATA);
        ui.modeLabel->setText("Mode: " + this->getModeToString());
        QMessageBox::information(this, "Course", "La course est en mode DATA !");
    } else {
        QMessageBox::information(this, "Course", "Assurez-vous que le client communique avec le lecteur !");
    }
}

QString GestionPortique::getModeToString()
{
    if(RaceManager::getInstance()->getMode() == RaceManager::DATA)
    {
        return "DATA";
    }
    return "RAZ";
}

