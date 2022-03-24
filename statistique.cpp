#include "statistique.h"
#include "ui_statistique.h"
#include <QSqlQueryModel>
#include <QSqlQuery>

Statistique::Statistique()
{
    ui.setupUi(this);
}


void Statistique::Init()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(QSqlQuery("SELECT participants.id AS participantId, "
                              "participants.genre_id AS genreId, "
                              "participants.firstname AS nom_participant "
                              "FROM participants "
                              ""));
    ui.statistiqueView->setModel(model);
}

Statistique::~Statistique()
{
}
