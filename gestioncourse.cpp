#include "databasemanager.h"
#include <QtSql>
#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QWidget>
#include <string.h>
#include <gestioncourse.h>

GestionCourse::GestionCourse()
{
    ui.setupUi(this);
}


void GestionCourse::init()
{
    this->createTableView();
    createMenuBar();
}


void GestionCourse::createTableView()
{
    // Creation du model
    model = new QSqlRelationalTableModel(ui.courseTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("races");

    genreIdx = model->fieldIndex("genre_id");
    model->setRelation(genreIdx, QSqlRelation("genders", "id", "sexe"));

    model->setHeaderData(model->fieldIndex("name"), Qt::Horizontal, tr("Course"));

    if (!model->select()) {
        QMessageBox::critical(this, "Base de données n'a pas été initialiser correctement",
                    "Erreur: " + model->lastError().text());
        return;
    }

    ui.courseTable->setModel(model);
    ui.courseTable->setItemDelegate(new QSqlRelationalDelegate(ui.courseTable));
    ui.courseTable->setColumnHidden(model->fieldIndex("id"), true);
    ui.courseTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void GestionCourse::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quitter"), this);
    connect(quitAction, &QAction::triggered, this, &GestionCourse::close);
}



GestionCourse::~GestionCourse()
{
}

void GestionCourse::on_continueButton_clicked()
{
    QModelIndexList selectedIndexes = ui.courseTable->selectionModel()->selectedIndexes();

    QSqlQuery q;
   // q.prepare("UPDATE participants SET lastname=:lastname, firstname=:firstname, mail=:mail, password=:password, year=:year, genre_id=:genre_id, rfid=:rfid WHERE id=:id");
    q.bindValue(":id", selectedIndexes[0].data().value<int>());
    qDebug() << q.exec();
    this->createTableView();
}

