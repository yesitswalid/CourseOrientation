#include "application.h"
#include "ui_application.h"
#include <gestionparticipant.h>
#include <inscriptionform.h>
#include <statistique.h>
#include <sqliteconverter.cpp>

Application::Application(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Application)
{
    ui->setupUi(this);


    SQLiteConverter *sqlite = new SQLiteConverter("course.db");
    qDebug() << sqlite->SqlDataToMap();

    //Gestion du participant

    this->gestion_participant = new GestionParticipant();
    this->gestion_participant->init();

    this->inscription_form = new InscriptionForm();


    this->statistique = new Statistique();


}


Application::~Application()
{
    delete ui;
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

