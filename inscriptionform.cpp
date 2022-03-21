#include "inscriptionform.h"
#include "ui_inscriptionform.h"
#include <databasemanager.h>
#include <QtSql>
#include <QDate>

InscriptionForm::InscriptionForm()
{
    ui.setupUi(this);

    //1900 à 2022
    ui.dateEdit->setMinimumDate(QDate::currentDate().addYears(-100));
    ui.dateEdit->setMaximumDate(QDate::currentDate());

    //definit la selection du sexe
    ui.comboBox->insertItem(0, "M");
    ui.comboBox->insertItem(1, "Mme");
    ui.comboBox->insertItem(2, "Non binaire");
}


bool InscriptionForm::isFormEmpty()
{
    return (ui.nomEdit->text().isEmpty() || ui.prenomEdit->text().isEmpty() || ui.mailEdit->text().isEmpty() || ui.mdpEdit->text().isEmpty());
}


QString InscriptionForm::errorMessage()
{
    if(ui.nomEdit->text().isEmpty())
    {
        return QString("Vous n'avez pas remplie de nom !");
    }

    if(ui.prenomEdit->text().isEmpty())
    {
        return QString("Vous n'avez pas remplie de Prénom!");
    }

    if(!ui.dateEdit->date().isValid())
    {
       return QString("Vous n'avez pas remplie de date de naissance !");
    }

    if(ui.mailEdit->text().isEmpty())
    {
        return QString("Vous n'avez pas entrez d'adresse mail !");
    }

    if(ui.mdpEdit->text().isEmpty())
    {
        return QString("Vous n'avez pas entrez de mot de passe !");
    }

    return QString("Erreur lors du traitement du formulaire !");
}


void InscriptionForm::clearAllInputs()
{
    ui.nomEdit->clear();
    ui.prenomEdit->clear();
    ui.dateEdit->clear();
    ui.mailEdit->clear();
    ui.mdpEdit->clear();

}

void InscriptionForm::Register()
{
    //addParticipant(q, QString("Khazri2"), QString("Walid2"), QString("test133@gmail.com"), QString::fromLocal8Bit(QCryptographicHash::hash("mdp12399", QCryptographicHash::Sha256)), QDate::currentDate().toString("dd/MM/yyyy"), 1);
    m_db->addParticipant(ui.nomEdit->text().replace(" ", ""), ui.prenomEdit->text().replace(" ", ""), ui.mailEdit->text().replace(" ", ""), QString::fromLocal8Bit(QCryptographicHash::hash(ui.mdpEdit->text().toLocal8Bit(), QCryptographicHash::Sha256)), ui.dateEdit->text(), this->getSexe());
    QMessageBox::information(this, "Inscription reussi",
                "Le participant: " + ui.nomEdit->text().replace(" ", "") + " " + ui.prenomEdit->text().replace(" ", "") + " est desormais inscrit !");
    this->clearAllInputs();
}


int InscriptionForm::getSexe()
{
   int sexe;
   switch(ui.comboBox->currentIndex())
   {
   case 0:
       sexe = 1;
       break;
   case 1:
       sexe = 2;
       break;
   case 2:
       sexe = 3;
       break;
   default:
       sexe = 1;

   }
   return sexe;
}


InscriptionForm::~InscriptionForm()
{

}


void InscriptionForm::on_inscriptionButton_clicked()
{
    if(this->isFormEmpty())
    {
        QMessageBox::critical(this, "Erreur lors de l'inscription du participant",
                    "Erreur: " + errorMessage());
    } else {
        if(!m_db->isParticipantExist(ui.mailEdit->text().replace(" ", "")))
        {
           this->Register();
        } else {
            QMessageBox::critical(this, "Inscription",
                        "Un participant possède déjà cette adresse mail !");
        }
    }
}

