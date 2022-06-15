#include "application.h"
#include "ui_application.h"
#include <gestionparticipant.h>
#include <gestionportique.h>
#include <inscriptionform.h>
#include <SQLiteConverter.cpp>
#include <mysqldata.cpp>
#include <QFileInfo>
#include <racemanager.h>
#include <QDateTime>

void Application::init()
{

    //Initialisation des données
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                    this,
                    "Impossible de charger la base de données sqlite",
                    "Le pilote SQLite n'a pas été trouver"
                    );

     m_db = new DatabaseManager();


    // Initialisation base de données
    QSqlError err = m_db->initDb();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

    //Initialisation de la config
    configuration = new Configuration();

    //Initialisation de la classe qui permettra d'exporter, convertir les requetes et de recuperer les informations.
    m_sqlite = new SQLiteConverter("course.db");

    //QString hostname, QString dbName, QString username, QString password


    //Gestion du participant

    this->gestion_participant = new GestionParticipant(m_db);

    //Inscription Formulaire

    this->inscription_form = new InscriptionForm(m_db);



    //Config Base de données
    this->config_form = new ConfigForm();

    //Gestion Portique
    this->gestion_portique = new GestionPortique();


    QJsonObject m_obj_values = this->config_form->getDatabaseData();

    //Recuperer les valeurs depuis le config.json valeur de la base de données pour la connexion

    if(m_obj_values.contains("ip") && m_obj_values.contains("port") &&
            m_obj_values.contains("user") && m_obj_values.contains("mot_de_passe")
            && m_obj_values.contains("database"))
    {
         m_mydb = new MySQLData(m_obj_values["user"].toString(),
                 m_obj_values["mot_de_passe"].toString(),
                 m_obj_values["ip"].toString(),
                 m_obj_values["database"].toString());

    }
    else
    {
        m_mydb = new MySQLData("admin", "73L5j28vddk3uTK", "1172.16.10.22", "coursorient");
    }

    this->config_form->init(m_mydb);

    initRaces();

}



Application::Application(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Application)
{
    ui->setupUi(this);

    this->init();
}


Application::~Application()
{
    //Nettoyer la memoire après la fermuture de l'application
    delete ui;
    delete gestion_participant;
    delete inscription_form;
    delete config_form;
    delete configuration;
    //delete m_db;
   // delete m_sqlite;
    delete m_mydb;
}


void Application::initRaces()
{
    QSqlQuery query;

    struct RaceManager::Race race;

    QVector<RaceManager::Race> races;

    query = m_db->getDb().exec();
    query.prepare(QString("SELECT * FROM races"));
    if (!query.exec())
    {
        return;
    }

    while(query.next())
    {
        int race_id = query.value(0).toInt();
        int id_department = query.value(1).toInt();
        QString raceName = query.value(2).toString();
        QString date = query.value(3).toString();
        QString location = query.value(4).toString();
        float gps_longitude = query.value(5).toFloat();
        float gps_latitude = query.value(6).toFloat();
        int difficulty = query.value(7).toInt();
        int type = query.value(8).toInt();
        int book = query.value(9).toInt();

        race.name = raceName;
        race.raceId = race_id;
        race.departmentId = id_department;
        race.date = date;
        race.location = location;
        race.gpsLongitude = gps_longitude;
        race.gpsLatitude = gps_latitude;
        race.difficulty = difficulty;
        race.type = type;
        race.book = book;


        races.push_back(race);

        //RaceManager::getInstance()->addRace(race);

    }


    RaceManager::getInstance()->setRaces(races);

    ui->comboBox->clear();

    ui->comboBox->addItem("-");
    int i = 0;
    comboLists.insert(0, "-");
    for(const struct RaceManager::Race &r : races)
    {
        i++;      
        ui->comboBox->addItem(QDateTime::fromString(r.date, Qt::ISODate).toLocalTime().toString("yyyy/MM/dd hh:mm:ss") + " "+ r.name);
        comboLists.insert(i, r.date);
        // Remplir le tableau avec les id des races
    }
}



void Application::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Base de données n'a pas été initialiser correctement",
                "Erreur: " + err.text());
}


void Application::on_actionConfiguration_triggered()
{

}


void Application::on_actionInscription_triggered()
{
    if(RaceManager::getInstance()->isRaceSelected())
    {
         this->inscription_form->show();
    } else {
         QMessageBox::warning(this, "Inscription", "Vous devez selectionner une course pour pouvoir accèder à cette page !");
    }
}


void Application::on_actionGestion_des_participants_triggered()
{
     this->gestion_participant->createTableView();
     this->gestion_participant->show();
}

void Application::on_actionBddConfig_triggered()
{
    this->config_form->show();
}


void Application::on_actionExporter_triggered()
{
    if(RaceManager::getInstance()->isRaceSelected())
    {
        if(m_mydb->exportData())
        {
            ///
            /// \brief QMessageBox::information
            ///
            ///
             QMessageBox::information(this, "Exportation", "L'exportation des données des participants on été envoyée au serveur web !");



        } else {
             QMessageBox::warning(this, "Exportation", "Erreur lors de l'exportation des données vérifier la base de données du serveur web !");
        }
    } else {
         QMessageBox::warning(this, "Inscription", "Vous devez selectionner une course pour pouvoir effectuer cette action !");
    }
}


void Application::on_actionImporter_triggered()
{
    if(m_mydb->importData()){

        initRaces();
        QMessageBox::information(this, "Importation", "Importation avec succès des données du site web !");
    } else {
        QMessageBox::warning(this, "Importation", "Erreur de l'importation vérifier la base de données du serveur web !");
    }
}


void Application::on_buttonSelectRace_clicked()
{

    //Savoir grace a l'index pour chaque course si dans une course la date correspond a la chaine de caractere de la date de la combolist
    //et affecter les parametres de la course à l'application de l'organisateur.

    for(const struct RaceManager::Race &race : RaceManager::getInstance()->getRaces())
    {

        if(race.date == comboLists.at(ui->comboBox->currentIndex()))
        {
            RaceManager::getInstance()->setRaceSelected(true);
            RaceManager::getInstance()->setRace(race.raceId, race.departmentId, race.name,
                                                race.date, race.location, race.gpsLongitude, race.gpsLatitude,
                                                race.difficulty, race.type, race.book);
            QMessageBox::information(this, "Course", "Vous avez selectionner la course: " + race.name);


            //action fichier
            for(QAction *ac : ui->menuParam_tre->actions())
            {
                if(!ac->isEnabled())
                {
                    ac->setEnabled(true);
                }
            }

            //action fenetre
            for(QAction *action : ui->menuFen_tre->actions())
            {
                if(!action->isEnabled())
                {
                    action->setEnabled(true);
                }
            }
        }
    }
}


void Application::on_actionPortique_triggered()
{
    if(RaceManager::getInstance()->isRaceSelected())
    {
         this->gestion_portique->show();
    } else {
         QMessageBox::warning(this, "Gestion Portique", "Vous devez selectionner une course pour pouvoir accèder à cette page !");
    }
}


void Application::on_actionQuitter_2_triggered()
{
    this->close();
}




void Application::on_actionReinitialiser_les_donn_es_triggered()
{
    if(RaceManager::getInstance()->isRaceSelected())
    {

        QSqlQuery q = m_db->getDb().exec();

        //suppression des lignes de la base de données local de chaque tables correspondant a la course selectionné
        q.prepare("DELETE FROM TABLE participant_races WHERE race_id=?");
        q.addBindValue(RaceManager::getInstance()->getRaceId());
        q.exec();
        q.exec("DELETE FROM TABLE participant_races_data WHERE race_id=?");
        q.addBindValue(RaceManager::getInstance()->getRaceId());
        q.exec();
        q.exec("DELETE FROM TABLE checkpoints WHERE race_id=?");
        q.addBindValue(RaceManager::getInstance()->getRaceId());
        q.exec();

        QMessageBox::information(this, "Reinitialisation", "Vous avez reinitiliser la course avec succès !");
    } else {
         QMessageBox::warning(this, "Reinitialisation", "Vous devez selectionner une course pour pouvoir supprimer les données d'une course !");
    }
}

