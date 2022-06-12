#include "gestionparticipant.h"
#include "ui_gestionparticipant.h"
//#include <QThread>
//#include <portique.h>
#include <configuration.cpp>
#include "racemanager.h"
#include "databasemanager.h"
#include <QtSql>
#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string.h>


GestionParticipant::GestionParticipant()
{
    m_db = new DatabaseManager();
    ui.setupUi(this);



    //// CREATION DU MENU QUITTER ////
    ///
    this->createMenuBar();

    ////////////////////////////////


    ///////////// PARTIE SIMULATION /////////////////
    ///


    this->sim_config = new Configuration("simulation.json");

    //////////////////////////////////////////////////////



    ///////////////// GESTION PORTIQUE //////////////////


    serial = new QSerialPort();
    serial->setPortName("/dev/ttyUSB0");
    if(!serial->setBaudRate(QSerialPort::Baud9600))
    {
        qDebug() << "Erreur lors de l'initialisation du baude rate à 9600";
        return;
    }

    if(!serial->setStopBits(QSerialPort::OneStop))
    {
        qDebug() << "Erreur lors de l'initialisation du stop bit à 1";
        return;
    }

    if(!serial->setDataBits(QSerialPort::Data8))
    {
        qDebug() << "Erreur lors de l'initialisation à 8 bits";
        return;
    }

    if(!serial->setFlowControl(QSerialPort::NoFlowControl))
    {
        qDebug() << "Erreur lors de l'initialisation du flux de controle en non flux de controle";
    }

    if(serial->open(QIODevice::ReadOnly))
    {
        connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));
        //connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));

    } else {
        qDebug() << "Erreur lors de la lecture !" << serial->error();
    }

}

GestionParticipant::GestionParticipant(DatabaseManager *db)
{
    m_db = db;
    ui.setupUi(this);

    //// CREATION DU MENU QUITTER ////
    ///
    this->createMenuBar();

    ///////////// PARTIE SIMULATION /////////////////

    this->sim_config = new Configuration("simulation.json");


    ////////////////////////////////////////



    ////////// GESTION PORTIQUE /////////////////////


    serial = new QSerialPort();
    serial->setPortName("/dev/ttyUSB0");
    if(!serial->setBaudRate(QSerialPort::Baud9600))
    {
        qDebug() << "Erreur lors de l'initialisation du baude rate à 9600";
        return;
    }

    if(!serial->setStopBits(QSerialPort::OneStop))
    {
        qDebug() << "Erreur lors de l'initialisation du stop bit à 1";
        return;
    }

    if(!serial->setDataBits(QSerialPort::Data8))
    {
        qDebug() << "Erreur lors de l'initialisation à 8 bits";
        return;
    }

    if(!serial->setFlowControl(QSerialPort::NoFlowControl))
    {
        qDebug() << "Erreur lors de l'initialisation du flux de controle en non flux de controle";
    }

    if(serial->open(QIODevice::ReadOnly))
    {
        connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));
        //connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));

    } else {
        qDebug() << "(Erreur lors de la lecture) Erreur Précise: " << serial->error();
    }
}


void GestionParticipant::createTableView()
{


    // Creation du model
    model = new QSqlRelationalTableModel(ui.participantTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("participants");


    QSqlQuery q = m_db->getDb().exec();
    q.prepare("SELECT p.id FROM "
                        "participants as p, participant_races as pr"
                        " WHERE p.id=pr.participant_id and pr.race_id="+QString::number(RaceManager::getInstance()->getRaceId()));

    if (q.exec())
    {
        QString ids = "(";
        while(q.next())
        {
            ids += q.value(0).toString() + ",";
        }

        //permet de récupérer la derniere chaine de caractere , pour la remplacer en )
        //pour refermer la requete sql executer un statement  et recuperer les ids des participants de la course

        //Sortie du resultat
        // qDebug() << ids;

        //Exemple: apres que la boucle est finit le resultat : (1,5,
        //Après l'avoir traiter (1,5) en referme le statement sinon cela declanche une erreur de la requete.
        ids = ids.left(ids.lastIndexOf(',')).append(")");

       //Sortie du resultat final
       // qDebug() << ids;

        //Execution de la requete du statement.
        model->setFilter("participants.id in " + ids);
    }

    genreIdx = model->fieldIndex("genre_id");
    model->setRelation(genreIdx, QSqlRelation("genders", "id", "sexe"));

    //Specifier les colonnes des particpant necessaire pour l'organisateur pour la modification.
    model->setHeaderData(model->fieldIndex("lastname"), Qt::Horizontal, tr("Nom"));
    model->setHeaderData(model->fieldIndex("firstname"), Qt::Horizontal, tr("Prénom"));
    model->setHeaderData(model->fieldIndex("mail"), Qt::Horizontal, tr("Mail"));
    model->setHeaderData(model->fieldIndex("password"), Qt::Horizontal, tr("Mot de Passe"));
    model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Date de Naissance"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));

    //en cas d'erreur si la colonne de la table participant n'existe pas cela retournera un MessageBox d'erreur sql detailler
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    //Appliquer le model de la vue du tableau
    ui.participantTable->setModel(model);
    //Appliquer
    ui.participantTable->setItemDelegate(new QSqlRelationalDelegate(ui.participantTable));
    //Cacher la colonne id dans le model du tableau
    ui.participantTable->setColumnHidden(model->fieldIndex("id"), true);
    //Activer la selection du model pour permettre la selection des items
    ui.participantTable->setSelectionMode(QAbstractItemView::SingleSelection);

    //la relation de la table participants avec le genre_id determine grace a l'id le sexe du participant.
    //et s'appliquera sur le model et ainsi que sur la colonne du Genre du participant
    ui.genreEdit->setModel(model->relationModel(genreIdx));
    ui.genreEdit->setModelColumn(model->relationModel(genreIdx)->fieldIndex("sexe"));


    //La classe QDataWidgetMapper fournit un mappage entre une section d'un modèle de données et des widgets.
    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    //Appliquer le model QSqlRelationTableModel
    mapper->setModel(ui.participantTable->model());

    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    //Element necessaire du mappage pour l'édition du model de vue fournis à l'organisateur
    mapper->addMapping(ui.nomEdit, model->fieldIndex("lastname"));
    mapper->addMapping(ui.prenomEdit, model->fieldIndex("firstname"));
    mapper->addMapping(ui.mailEdit, model->fieldIndex("mail"));
    mapper->addMapping(ui.genreEdit, genreIdx);
    mapper->addMapping(ui.dateEdit, model->fieldIndex("year"));

    //Appliquer un signal lorsqu'une valeur à été changer dans le model du tableau
    connect(ui.participantTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
            );

    //definir la selection de l'index par default de la ligne et de la colonne donc ligne 0 et colonne 0
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
    //Recuperer toutes les valeurs situes dans le tableau et récuperer seulement la ligne selectionner pour pouvoir récuperer les valeur de cette ligne.
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();
    m_db->removeParticipant(selectedIndexes[0].data().value<int>());  //Enlever le participant grace à l'id
    this->createTableView(); //Creation du tableau et raffraichissement du model
}


void GestionParticipant::on_updateButton_clicked()
{
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();


    if(!selectedIndexes.isEmpty())
    { //ceci évite le crash de l'application et permet de savoir si l'index contient une selection

        QSqlQuery q;

        //Préparer la requete pour mettre à jour les information du participant
        q.prepare("UPDATE participants SET lastname=:lastname, "
                  "firstname=:firstname,"
                  "mail=:mail,"
                  "password=:password,"
                  "year=:year,"
                  "genre_id=:genre_id"
                  " WHERE id=:id");
        //récupérer toutes les valeur du model du tableau de chaque colonne
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

        //q.bindValue(":rfid", selectedIndexes[7].data().value<int>());
        if(q.exec()) //verification True = la requete a bien été mis à jour ou false erreur du sql
        {
            QMessageBox::information(this, "Gestion des Participants", "Le participant : "
             "" + selectedIndexes[1].data().value<QString>() + " "
             "" + selectedIndexes[2].data().value<QString>() + " à été mis à jour avec succès !");
            this->createTableView(); //recréer le model du tableau pour rafraichir les mise à jour éffectuer des participants
        } else {
            QMessageBox::warning(this, "Gestion des Participants", "Erreur lors de la mise à jour du participant");
        }
     }
}

void GestionParticipant::serialReceived()
{

    while(serial->canReadLine())
    {
        QByteArray data = serial->readLine();

        if(!ui.razButton->isEnabled())
        {
            QString id = "";
            for(int i = 7; i < data.length(); i++)
            {
                id += data[i];

            }
            id.replace("\r\n", "");

            qDebug() << id << QDateTime::currentSecsSinceEpoch();
            ui.razButton->setEnabled(true);


        }
    }
}


void GestionParticipant::razGetData(QByteArray data)
{
    qDebug() << data;
    qDebug() << "getter";
}

void GestionParticipant::finishedRead()
{
    if(!ui.razButton->isEnabled())
    {
        ui.razButton->setEnabled(true);
    }
}

void GestionParticipant::on_razButton_clicked()
{
    //Recuperer toutes les valeurs situes dans le tableau et récuperer seulement la ligne selectionner pour pouvoir récuperer les valeur de cette ligne.
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();
    qDebug() << selectedIndexes[0].data(); //Afficher l'id du récuperation de l'id du participant (Test debug pour afficher l'id supprimer)

    if(ui.razButton->isEnabled())
    {
         ui.razButton->setEnabled(false);

    }

}


/////////////////////// PARTIE SIMULATION ////////////////////////////////////////////
/// \brief GestionParticipant::on_razSimButton_clicked
///
/// PROTOTYPE

void GestionParticipant::on_razSimButton_clicked()
{
     RaceManager::getInstance()->setMode(RaceManager::RAZ);

     QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

     if(!m_db->isFingerExist(selectedIndexes[0].data().value<int>()) && !m_db->isPortiqueBIDExist(selectedIndexes[0].data().value<int>()))
     {

         QJsonObject obj = this->sim_config->getAll();

         QJsonValueRef m_values = obj.find(QString::number(this->participantSimId++)).value();
         //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
         QJsonObject m_obj_values = m_values.toObject();


         QVariant fingerId = m_obj_values.value("finger_id").toVariant();
         QVariant bidId = m_obj_values.value("passage_id").toVariant();

         qDebug() << fingerId << bidId;

         //int passage_start = m_obj_values.value("passage_start").toInt();

         m_db->setFinger(selectedIndexes[0].data().value<int>(), fingerId.toLongLong());
         m_db->setPortiqueBID(selectedIndexes[0].data().value<int>(), bidId.toLongLong());


         //qDebug() << m_db->getParticipantData(selectedIndexes[0].data().value<int>());

         QMessageBox::information(this, "Participant", "le participant à bien démarré la course !");
     } else {
          QMessageBox::warning(this, "RAZ", "Ce participant possède déjà un numéro inscrit sur le RFID ainsi que sur le doigt");
     }
}


void GestionParticipant::on_dataSimButton_clicked()
{
    RaceManager::getInstance()->setMode(RaceManager::DATA);


    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    if(m_db->isFingerExist(selectedIndexes[0].data().value<int>()) && m_db->isPortiqueBIDExist(selectedIndexes[0].data().value<int>()))
    {

        QJsonObject obj = this->sim_config->getAll();


        QSqlQuery q = m_db->getDb().exec();

        q.prepare("SELECT finger FROM participant_races WHERE participant_id=?");
        q.addBindValue(selectedIndexes[0].data().value<int>());
        qDebug() << q.exec();

        while(q.next())
        {
            //qDebug() << q.value(0);


            for(int i = 1; i < 5; i++)
            {

               QJsonValueRef m_values = obj.find(QString::number(i)).value();


               //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
               QJsonObject m_obj_values = m_values.toObject();


               if(m_obj_values.value("finger_id").toVariant().toLongLong() == q.value(0).toLongLong())  //Trouver les données (JsonObject) du participantà l'aide de l'id du doigt(RFID)
               {
                   qDebug() << m_obj_values;

               }
               //int passageId = m_obj_values.value("passage_id").toInt();
               //int passage_start = m_obj_values.value("passage_start").toInt();
           }
        }
    }
}


