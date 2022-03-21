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
    model->setQuery(QSqlQuery("select * from participants"));
    ui.statistiqueView->setModel(model);
}

Statistique::~Statistique()
{
}
