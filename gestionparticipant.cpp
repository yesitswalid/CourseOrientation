#include "gestionparticipant.h"
#include "ui_gestionparticipant.h"
//#include "database_manager.h"
#include "databasemanager.h"
#include <QtSql>
#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QWidget>
#include <string.h>

GestionParticipant::GestionParticipant()
{
    ui.setupUi(this);
}


void GestionParticipant::init()
{
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                    this,
                    "Impossible de charger la base de données sqlite",
                    "Le pilote SQLite n'a pas été trouver"
                    );

    // Initialisation base de données
    QSqlError err = m_db->initDb();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

    this->createTableView();
    createMenuBar();
}


void GestionParticipant::createTableView()
{
    // Creation du model
    model = new QSqlRelationalTableModel(ui.participantTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("participants");

    genreIdx = model->fieldIndex("genre_id");
    model->setRelation(genreIdx, QSqlRelation("genders", "id", "sexe"));

    model->setHeaderData(model->fieldIndex("lastname"), Qt::Horizontal, tr("Nom"));
    model->setHeaderData(model->fieldIndex("firstname"), Qt::Horizontal, tr("Prénom"));
    model->setHeaderData(model->fieldIndex("mail"), Qt::Horizontal, tr("Mail"));
    model->setHeaderData(model->fieldIndex("password"), Qt::Horizontal, tr("Mot de Passe"));
    model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Date de Naissance"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));
    model->setHeaderData(model->fieldIndex("rfid"), Qt::Horizontal, tr("RFID"));

    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    ui.participantTable->setModel(model);
    ui.participantTable->setItemDelegate(new QSqlRelationalDelegate(ui.participantTable));
    ui.participantTable->setColumnHidden(model->fieldIndex("id"), true);
    ui.participantTable->setSelectionMode(QAbstractItemView::SingleSelection);

    ui.genreEdit->setModel(model->relationModel(genreIdx));
    ui.genreEdit->setModelColumn(model->relationModel(genreIdx)->fieldIndex("sexe"));

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(ui.participantTable->model());
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui.nomEdit, model->fieldIndex("lastname"));
    mapper->addMapping(ui.prenomEdit, model->fieldIndex("firstname"));
    mapper->addMapping(ui.mailEdit, model->fieldIndex("mail"));
    mapper->addMapping(ui.genreEdit, genreIdx);
    mapper->addMapping(ui.dateEdit, model->fieldIndex("year"));
    mapper->addMapping(ui.rfidEdit, model->fieldIndex("rfid"));


    connect(ui.participantTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
            );

    ui.participantTable->setCurrentIndex(model->index(0, 0));
}

void GestionParticipant::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quitter"), this);
    connect(quitAction, &QAction::triggered, this, &GestionParticipant::close);
}

void GestionParticipant::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Base de données n'a pas été initialiser correctement",
                "Erreur: " + err.text());
}


GestionParticipant::~GestionParticipant()
{
}

void GestionParticipant::on_suprButton_clicked()
{
    //ui.participantTable->currentIndex().column()
    //ui.participantTable->model()->
    //qDebug() << ui.participantTable->model()->removeRow(ui.participantTable->currentIndex().row());
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();
    qDebug() << selectedIndexes[0].data();
    m_db->removeParticipant(selectedIndexes[0].data().value<int>());
    this->createTableView();


    /*
    for(auto i = selectedIndexes.constBegin();i!=selectedIndexes.constEnd();++i){
        qDebug() << i->data();

    }

    */

    //qDebug() << ui.participantTable->selectionModel()->selectedRows(ui.participantTable->currentIndex().column())[0].row();

            //ui.participantTable->model()->removeRows(ui.participantTable->currentIndex().row(), 1);
}


void GestionParticipant::on_updateButton_clicked()
{
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();
    QSqlQuery q;
    q.prepare("UPDATE participants SET lastname=:lastname, firstname=:firstname, mail=:mail, password=:password, year=:year, genre_id=:genre_id, rfid=:rfid WHERE id=:id");
    q.bindValue(":id", selectedIndexes[0].data().value<int>());
    q.bindValue(":lastname", selectedIndexes[1].data().value<QString>());
    q.bindValue(":firstname", selectedIndexes[2].data().value<QString>());
    q.bindValue(":mail", selectedIndexes[3].data().value<QString>());
    q.bindValue(":password", selectedIndexes[4].data().value<QString>());
    q.bindValue(":year", selectedIndexes[5].data().value<QString>());

    QString genre = selectedIndexes[6].data().value<QString>();
    qDebug() << genre;
    if(genre == "masculin")
    {
       q.bindValue(":genre_id", 1);
    } else if(genre == "feminin")
    {
        q.bindValue(":genre_id", 2);
    } else {
        q.bindValue(":genre_id", 3);
    }

    q.bindValue(":rfid", selectedIndexes[7].data().value<int>());
    /*
    for(auto i = selectedIndexes.constBegin() + 1;i!=selectedIndexes.constEnd();++i){
        q.addBindValue(i->data().toString());
        qDebug() << i->data().toString();
    }
    qDebug() << selectedIndexes[0].data().value<int>();
    q.addBindValue(selectedIndexes[0].data().value<int>());
    qDebug() << q.exec();
    */
    qDebug() << q.exec();
    this->createTableView();

}

