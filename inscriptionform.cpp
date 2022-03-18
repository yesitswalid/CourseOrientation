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
}


bool InscriptionForm::isFormEmpty()
{
    return ((ui.nomEdit->text().isEmpty() || ui.prenomEdit->text().isEmpty() || ui.mailEdit->text().isEmpty() || ui.mdpEdit->text().isEmpty() || !ui.dateEdit->date().isValid()));
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

    if(ui.mailEdit->text().isEmpty())
    {
       return QString("Vous n'avez pas remplie de mail !");
    }

    if(ui.dateEdit->date().isValid())
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


bool InscriptionForm::isParticipantExist(const QString &mail)
{
     QSqlQuery q;
     q.prepare(SELECT_DATA_PARTICIPANT);
     q.addBindValue(mail);
     if(q.exec())
     {
        return q.next();
     }
     return false;
}

void InscriptionForm::Register()
{
    //addParticipant(q, QString("Khazri2"), QString("Walid2"), QString("test133@gmail.com"), QString::fromLocal8Bit(QCryptographicHash::hash("mdp12399", QCryptographicHash::Sha256)), QDate::currentDate().toString("dd/MM/yyyy"), 1);
    m_db->addParticipant(ui.nomEdit->text().replace(" ", ""), ui.prenomEdit->text().replace(" ", ""), ui.mailEdit->text().replace(" ", ""), QString::fromLocal8Bit(QCryptographicHash::hash(ui.mdpEdit->text().toLocal8Bit(), QCryptographicHash::Sha256)), ui.dateEdit->text(), this->getSexe());
}


int InscriptionForm::getSexe()
{
   return 1;
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
    }
}

