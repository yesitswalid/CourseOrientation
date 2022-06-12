#include "ui_gestionportique.h"
#include "gestionportique.h"
#include "racemanager.h"


GestionPortique::GestionPortique()
{
    ui.setupUi(this);
}


GestionPortique::~GestionPortique()
{
}


void GestionPortique::on_razButton_clicked()
{
    RaceManager::getInstance()->setMode(RaceManager::RAZ);
}


void GestionPortique::on_dataButton_clicked()
{
      RaceManager::getInstance()->setMode(RaceManager::DATA);
}

