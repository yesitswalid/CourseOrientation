#include "application.h"
#include "ui_application.h"
#include <gestionparticipant.h>
#include <inscriptionform.h>
#include <statistique.h>
#include <sqliteconverter.cpp>

void Application::init()
{
    //Iniatilisation des données
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

   // SQLiteConverter *sqlite = new SQLiteConverter("course.db");
    //qDebug() << sqlite->SqlDataToMap();


    //Gestion du participant

    this->gestion_participant = new GestionParticipant();
    this->gestion_participant->init();

    //Inscription Formulaire

    this->inscription_form = new InscriptionForm();

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
    delete ui;
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

