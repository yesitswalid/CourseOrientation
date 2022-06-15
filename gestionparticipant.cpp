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
#include "server.h"

GestionParticipant::GestionParticipant()
{
    m_db = new DatabaseManager();
    ui.setupUi(this);


    //Connecter le signal pour recevoir l'id de la carte
    connect(Server::getInstance(), SIGNAL(getCardId(QString)), this, SLOT(getCardId(QString)));

    //Connecter le signal pour recevoir les données de la course
    connect(Server::getInstance(), SIGNAL(getCardId(getResultDataParticipant(QString, int, int))), this, SLOT(getResultDataParticipant(QString, int, int)));


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


    //Connecter le signal pour recevoir l'id de la carte
    connect(Server::getInstance(), SIGNAL(getCardId(QString)), this, SLOT(getCardId(QString)));

    //Connecter le signal pour recevoir les données de la course
    connect(Server::getInstance(), SIGNAL(getResultDataParticipant(QString, int, int)), this, SLOT(getResultDataParticipant(QString, int, int)));

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



///// PARTIE RAZ RECUPERATION DE LA CARTE ID ET IDENTIFIER LE PARTICIPANT /////
/// \brief GestionParticipant::getCardId
/// \param cardId
///
void GestionParticipant::getCardId(QString cardId)
{
    qDebug() << "Depuis Gestion Particpant: " << cardId;


    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    if(!selectedIndexes.isEmpty())
    {


            if(RaceManager::getInstance()->getMode() == RaceManager::RAZ)
            {

                if(!ui.doigtButton->isEnabled())
                {

                    qDebug() << "======== LECTEUR RFID ========";
                    qDebug() << "Mode: RAZ";
                    qDebug() << "BADGE ID:" << cardId;

                    QSqlQuery q = m_db->getDb().exec();

                    q.prepare("SELECT firstname, lastname FROM participants WHERE id=?");
                    q.addBindValue(selectedIndexes[0].data().value<int>());
                    q.exec();

                    m_db->setFinger(selectedIndexes[0].data().value<int>(), cardId);

                    ui.doigtButton->setEnabled(true);

                    while(q.next())
                    {
                        qDebug() << "Participant: " << q.value(0).toString() << q.value(1).toString();
                        QMessageBox::information(this, "Lecteur", "Le participant: " + q.value(0).toString() + " " + q.value(1).toString() + " à scanner son badge: " + cardId);
                    }

                    //m_db->setDepartTimeParticipant(selectedIndexes[0].data().value<int>(), QDateTime::currentDateTime());

                    qDebug() << "======================";
                }

            }
    } else {
        QMessageBox::warning(this, "Lecteur", "Vous n'avez pas selectionné le participant !");
    }

}


void GestionParticipant::getResultDataParticipant(QString carteId, int pointsTotal, int nbBalises)
{

    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    if(RaceManager::getInstance()->isRaceSelected())
    {

         if(!ui.doigtButton->isEnabled())
         {

                    qDebug() << "======== LECTEUR RFID ========";
                    qDebug() << "Mode: DATA";
                    qDebug() << "BADGE ID: " << carteId;

                    QSqlQuery q = m_db->getDb().exec();

                    q.prepare("SELECT firstname, lastname FROM participants WHERE id=?");
                    q.addBindValue(selectedIndexes[0].data().value<int>());
                    q.exec();

                    while(q.next())
                    {
                        qDebug() << "PARTICIPANT ID: " <<  selectedIndexes[0].data().value<int>();
                        m_db->setPartipantPoints(selectedIndexes[0].data().value<int>(), pointsTotal);
                        m_db->setPartipantBeacons(selectedIndexes[0].data().value<int>(), nbBalises);
                        QMessageBox::information(this, "Lecteur", "Données recu de " + q.value(1).toString() + " " + q.value(2).toString() + " !");

                    }
          }

    } else {
        QMessageBox::warning(this, "Lecteur", "Vous n'avez pas selectionné de course (Impossible d'obtenir les données du participant) !");
     }

    ui.doigtButton->setEnabled(true);
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


////// PARTIE MAMOUN PORTIQUE (RECUPERATION DE DONNEES) ////////////

void GestionParticipant::serialReceived()
{

    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    if(RaceManager::getInstance()->getMode() == RaceManager::RAZ)
    {

    if(RaceManager::getInstance()->isRaceSelected())
    {


        while(serial->canReadLine())
        {

            QByteArray data = serial->readLine();

                qDebug() << "Test";

                if(!ui.portiqueButton->isEnabled())
                {
                    qDebug() << "MDrrr";

                    QString id = "";
                    for(int i = 7; i < data.length(); i++)
                    {
                        id += data[i];

                    }
                    id.replace("\r\n", "");

                    qDebug() << "======== PORTIQUE ========";
                    qDebug() << "Mode: RAZ";
                    qDebug() << "Dossard ID:" << id << "Timestamp: " <<  QDateTime::currentSecsSinceEpoch();
                    qDebug() << "DEPART: " << QDateTime::fromSecsSinceEpoch(QDateTime::currentSecsSinceEpoch()).toLocalTime().toString("yyyy/MM/dd hh:mm:ss");

                    qDebug() << "=======================";

                    if(!selectedIndexes.isEmpty())
                    {

                        m_db->setPortiqueBID(selectedIndexes[0].data().value<int>(), id);
                        m_db->setDepartTimeParticipant(selectedIndexes[0].data().value<int>(), QDateTime::currentDateTime());
                        QMessageBox::information(this, "Portique", "Départ du dossard: #" + id + " !");
                    } else {
                        QMessageBox::warning(this, "Portique", "Vous n'avez pas selectionné le participant !");
                    }

                    ui.portiqueButton->setEnabled(true);


                }
            }
           }
        } else if(RaceManager::getInstance()->getMode() == RaceManager::DATA)
        {
            while(serial->canReadLine())
            {

                QByteArray data = serial->readLine();


                if(!ui.portiqueButton->isEnabled())
                {
                    QString id = "";
                    for(int i = 7; i < data.length(); i++)
                    {
                        id += data[i];

                    }
                    id.replace("\r\n", "");

                    qDebug() << "======== PORTIQUE ========";
                    qDebug() << "Mode: RAZ";
                    qDebug() << "Dossard ID:" << id << "Timestamp: " <<  QDateTime::currentSecsSinceEpoch();
                    qDebug() << "ARRIVE: " << QDateTime::fromSecsSinceEpoch(QDateTime::currentSecsSinceEpoch()).toLocalTime().toString("yyyy/MM/dd hh:mm:ss");

                    qDebug() << "=======================";

                    if(!selectedIndexes.isEmpty())
                    {

                        m_db->setPortiqueBID(selectedIndexes[0].data().value<int>(), id);
                        m_db->setFinishTimeParticipant(selectedIndexes[0].data().value<int>(), QDateTime::currentDateTime());
                        QMessageBox::information(this, "Portique", "Arrive  du dossard: #" + id + " !");
                    } else {
                        QMessageBox::warning(this, "Portique", "Vous n'avez pas selectionné le participant !");
                    }

                    ui.portiqueButton->setEnabled(true);


                }
            }
           }
}



void GestionParticipant::on_portiqueButton_clicked()
{
    //Recuperer toutes les valeurs situes dans le tableau et récuperer seulement la ligne selectionner pour pouvoir récuperer les valeur de cette ligne.
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    if(!selectedIndexes.isEmpty())
    {
        qDebug() << selectedIndexes[0].data(); //Afficher l'id du récuperation de l'id du participant (Test debug pour afficher l'id supprimer)

        if(ui.portiqueButton->isEnabled())
        {
             ui.portiqueButton->setEnabled(false);

        }
    } else {
       QMessageBox::warning(this, "Selection", "Aucune selection n'a été éffectuer !");
    }
}

void GestionParticipant::on_doigtButton_clicked()
{
    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();
    if(!selectedIndexes.isEmpty())
    {
        if(ui.doigtButton->isEnabled())
        {
            ui.doigtButton->setEnabled(false);
        }
    } else {
      QMessageBox::warning(this, "Lecteur", "Vous n'avez pas selectionné le participant !");
      }
}



/////////////////////// PARTIE SIMULATION ////////////////////////////////////////////
/// \brief GestionParticipant::on_razSimButton_clicked
///
/// PROTOTYPE

void GestionParticipant::on_razSimButton_clicked()
{

     QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

     if(!selectedIndexes.isEmpty())
     {

         RaceManager::getInstance()->setMode(RaceManager::RAZ);

         if(!m_db->isFingerExist(selectedIndexes[0].data().value<int>()) && !m_db->isPortiqueBIDExist(selectedIndexes[0].data().value<int>()))
         {

             QJsonObject obj = this->sim_config->getAll();

             QJsonValueRef m_values = obj.find(QString::number(this->participantSimId++)).value();

             for(QString &key : obj.keys())
             {
                 QJsonObject m_obj = obj.find(key)->toObject();

                // if(m_obj.value(""))

             }


             //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
             QJsonObject m_obj_values = m_values.toObject();


             QVariant fingerId = m_obj_values.value("finger_id").toVariant();
             QVariant bidId = m_obj_values.value("bid_id").toVariant();

             qDebug() << fingerId << bidId;

             //int passage_start = m_obj_values.value("passage_start").toInt();


             //MODE RAZ (attribuer l'id du dossard et l'id du doigt(badge RFID)

             m_db->setFinger(selectedIndexes[0].data().value<int>(), fingerId.toString());
             m_db->setPortiqueBID(selectedIndexes[0].data().value<int>(), bidId.toString());


             //qDebug() << m_db->getParticipantData(selectedIndexes[0].data().value<int>());


             QSqlQuery q = m_db->getDb().exec();
             q.prepare("SELECT bid FROM participant_races WHERE participant_id=?");
             q.addBindValue(selectedIndexes[0].data().value<int>());
             q.exec();
             while(q.next())
             {
                 QString bid = q.value(0).toString();
                 QMessageBox::information(this, "Depart", "le participant du dossard #" + bid + " à bien démarré la course !");
             }

         } else {
              QMessageBox::warning(this, "RAZ", "Ce participant possède déjà un numéro de dossard inscrit ainsi que sur l'identifiant du doigt(Badge RFID)");

         }
     } else
     {
        QMessageBox::warning(this, "Selection", "Aucune selection n'a été éffectuer !");
     }
}



void GestionParticipant::on_dataSimButton_clicked()
{

    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    if(!selectedIndexes.isEmpty())
    {

        RaceManager::getInstance()->setMode(RaceManager::DATA);


        if(m_db->isFingerExist(selectedIndexes[0].data().value<int>()) && m_db->isPortiqueBIDExist(selectedIndexes[0].data().value<int>()))
        {

            QJsonObject obj = this->sim_config->getAll();


            QSqlQuery q = m_db->getDb().exec();

            q.prepare("SELECT finger FROM participant_races WHERE participant_id=?");
            q.addBindValue(selectedIndexes[0].data().value<int>());
            qDebug() << q.exec();

            while(q.next())
            {

                for(int i = 1; i < 5; i++)
                {


                   QJsonValueRef m_values = obj.find(QString::number(i)).value();


                   //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
                   QJsonObject m_obj_values = m_values.toObject();

                   //////////// TROUVER L'ID DU BADGE DU COUREUR ASSOCIER LORS DU MODE RAZ AVEC CELUI ENREGISTRER DANS LA BASE DE DONNEES POUR POUVOIR TRAITER LA BONNE DONNEES ////

                   if(m_obj_values.value("finger_id").toVariant().toLongLong() == q.value(0).toLongLong())  //Trouver les données (JsonObject) du participant à l'aide de l'id du doigt(RFID)
                   {
                       //qDebug() << m_obj_values;

                       /////////////////////////// PARTIE ENREGISTREMENT DES PASSAGES AU BALISES DANS LA BASE DE DONNEES /////////////////////////////////////
                       QJsonObject passages = m_obj_values.find("data")->toObject();

                       for(QString const &key : passages.keys())
                       {
                            QJsonObject passage_obj = passages.find(key)->toObject();
                            if(!m_db->hasParticipantCheckpoint(passage_obj.value("order_id").toInt(), RaceManager::getInstance()->getRaceId(), selectedIndexes[0].data().value<int>()))
                            {

                                m_db->addParticipantCheckpoint(passage_obj.value("altitude").toString(), passage_obj.value("longitude").toString(),
                                                               passage_obj.value("attitude").toString(), RaceManager::getInstance()->getRaceId(),
                                                               selectedIndexes[0].data().value<int>(),
                                        passage_obj.value("order_id").toInt(), passage_obj.value("points").toInt());
                            }
                       }

                       //////////////////////////////////////////////////////////////////////////////////////////////////////


                       //// PARTIE TRAITEMENT DE DONNEES APRES QUE LE COUREUR EST FINIT LA COURSE /////

                       int start = m_obj_values.value("bid_start").toInt();
                       int end = m_obj_values.value("bid_end").toInt();
                       float duration = (end - start) / 60; //convertir le timestamp de seconde en minute.

                       QSqlQuery q = m_db->getDb().exec();
                       q.prepare("SELECT bid FROM participant_races WHERE participant_id=?");
                       q.addBindValue(selectedIndexes[0].data().value<int>());
                       q.exec();
                       while(q.next())
                       {
                           QString bid = q.value(0).toString();
                           QMessageBox::information(this, "Arrivée", "Dossard: #" + bid + " Durée: " + QString::number(duration) + " minute(s) !");
                       }


                       //ENREGISTREMENT DATA


                   }
               }
            }
        }
    } else {
       QMessageBox::warning(this, "Selection", "Aucune selection n'a été éffectuer !");
    }
}


int GestionParticipant::getNewParticipantId()
{
    int newId = -1; // cas d'erreur

    QModelIndexList selectedIndexes = ui.participantTable->selectionModel()->selectedIndexes();

    QJsonObject obj = this->sim_config->getAll();


    QSqlQuery q = m_db->getDb().exec();

    q.prepare("SELECT finger FROM participant_races WHERE participant_id=?");
    q.addBindValue(selectedIndexes[0].data().value<int>());
    qDebug() << q.exec();

    while(q.next())
    {

        for(int i = 1; i < 5; i++)
        {


           QJsonValueRef m_values = obj.find(QString::number(i)).value();


           //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
           QJsonObject m_obj_values = m_values.toObject();

           //////////// TROUVER L'ID DU BADGE DU COUREUR ASSOCIER LORS DU MODE RAZ AVEC CELUI ENREGISTRER DANS LA BASE DE DONNEES POUR POUVOIR TRAITER LA BONNE DONNEES ////

           if(m_obj_values.value("finger_id").toVariant().toLongLong() == q.value(0).toLongLong())  //Trouver les données (JsonObject) du participant à l'aide de l'id du doigt(RFID)
           {
            }
        }

       }
    return newId;
}


