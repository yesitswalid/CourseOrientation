#include "statistique.h"
#include "ui_statistique.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <racemanager.h>

Statistique::Statistique()
{
    ui.setupUi(this);
}


void Statistique::init()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    int race_id = RaceManager::getInstance()->getRaceId();
    model->setQuery(QSqlQuery("SELECT participants.lastname AS Nom,"
                              "participants.firstname AS Prenom,"
                              "races.name as Course"
                              "FROM participants, participant_races, races, checkpoints WHERE participant_races.id=" + race_id));

    ui.statistiqueView->setModel(model);
}

Statistique::~Statistique()
{
}
