#include "application.h"
#include "ui_application.h"
#include <gestionparticipant.h>
#include <inscriptionform.h>
#include <statistique.h>
#include <sqliteconverter.cpp>
#include <mysqlconverter.cpp>
#include <QFileInfo>

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

    //ToDo : A changer mettre les valeur de la configuration check si les valeurs existe dans le fichier pour pouvoir les affecter
    m_mydb = new MySQLConverter("root", "walid13", "127.0.0.1", "coursorient");

    //qDebug() << m_mydb->SqlDataToMap();

    //Gestion du participant

    this->gestion_participant = new GestionParticipant();
    this->gestion_participant->init();

    //Inscription Formulaire

    this->inscription_form = new InscriptionForm();

    //Config Base de données
    this->config_form = new ConfigForm();

    //Statistique

    this->statistique = new Statistique();

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
    //Nettoyer la memoire après la fermuture de programme
    delete ui;
    delete gestion_participant;
    delete inscription_form;
    delete statistique;
    delete config_form;
    delete configuration;
    delete m_db;
    delete m_sqlite;
    delete m_mydb;
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
    this->inscription_form->show();
}


void Application::on_actionGestion_des_participants_triggered()
{
     this->gestion_participant->createTableView();
     this->gestion_participant->show();
}


void Application::on_actionStatistique_triggered()
{
    this->statistique->Init();
    this->statistique->show();
}


void Application::on_actionBddConfig_triggered()
{
    this->config_form->show();
}


void Application::on_actionExporter_vers_triggered()
{
    QString s = QFileDialog::getSaveFileName(this,
                    "Exporter vers (*.sql)",
                    "",
                    "sql (*.sql)");

    m_sqlite->exportSql(s);


}


void Application::on_actionImporter_triggered()
{
    QString s = QFileDialog::getExistingDirectory(this,
                    "Importer vers le dossier");
    if(m_mydb->importData(s)){
        QMessageBox::information(this, "Importation", "Importation avec succès les courses ont été sauvegarder dans le repertoire: " + s);
    } else {
        QMessageBox::warning(this, "Importation", "Erreur de l'importation vérifier la base de données du serveur web !");
    }
}
