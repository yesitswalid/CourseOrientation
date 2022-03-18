#include "gestionparticipant.h"
#include "ui_gestionparticipant.h"
//#include "database_manager.h"
#include "databasemanager.h"
#include <QtSql>
#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QWidget>

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

    // Creation du model
    model = new QSqlRelationalTableModel(ui.participantTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("participants");

    genreIdx = model->fieldIndex("genre_id");
    model->setRelation(genreIdx, QSqlRelation("genres", "id", "sexe"));

    model->setHeaderData(model->fieldIndex("lastname"), Qt::Horizontal, tr("Nom"));
    model->setHeaderData(model->fieldIndex("firstname"), Qt::Horizontal, tr("Prénom"));
    model->setHeaderData(model->fieldIndex("mail"), Qt::Horizontal, tr("Mail"));
    model->setHeaderData(model->fieldIndex("password"), Qt::Horizontal, tr("Mot de Passe"));
    model->setHeaderData(model->fieldIndex("date_birthday"), Qt::Horizontal, tr("Date de Naissance"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));

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
    mapper->setModel(model);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui.nomEdit, model->fieldIndex("nom"));
    mapper->addMapping(ui.prenomEdit, model->fieldIndex("prenom"));
    mapper->addMapping(ui.mailEdit, model->fieldIndex("mail"));
    mapper->addMapping(ui.genreEdit, genreIdx);


    connect(ui.participantTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
            );

    ui.participantTable->setCurrentIndex(model->index(0, 0));
    createMenuBar();
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





    /*
    for(auto i = selectedIndexes.constBegin();i!=selectedIndexes.constEnd();++i){
        qDebug() << i->data();

    }

    */

    //qDebug() << ui.participantTable->selectionModel()->selectedRows(ui.participantTable->currentIndex().column())[0].row();

            //ui.participantTable->model()->removeRows(ui.participantTable->currentIndex().row(), 1);
}

