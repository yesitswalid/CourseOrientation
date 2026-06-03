#include "inscriptionform.h"
#include "ui_inscriptionform.h"
#include "databasemanager.h"
#include <QtSql>
#include <QCryptographicHash>
#include <QDate>
#include <QMessageBox>

/* Initialise les champs communs aux deux constructeurs */
static void initialiserFormulaire(Ui::InscriptionForm &ui)
{
    /* Plage de dates autorisée : de 100 ans en arrière jusqu'à aujourd'hui */
    ui.dateEdit->setMinimumDate(QDate::currentDate().addYears(-100));
    ui.dateEdit->setMaximumDate(QDate::currentDate());

    /* Choix du genre */
    ui.comboBox->insertItem(0, "M");
    ui.comboBox->insertItem(1, "Mme");
    ui.comboBox->insertItem(2, "Non binaire");
}

InscriptionForm::InscriptionForm()
{
    ui.setupUi(this);
    m_db = new DatabaseManager();
    initialiserFormulaire(ui);
}

InscriptionForm::InscriptionForm(DatabaseManager *db)
{
    ui.setupUi(this);
    m_db = db;
    initialiserFormulaire(ui);
}

InscriptionForm::~InscriptionForm() {}

/* Retourne vrai si au moins un champ obligatoire est vide */
bool InscriptionForm::isFormEmpty()
{
    return ui.nomEdit->text().isEmpty()  ||
           ui.prenomEdit->text().isEmpty() ||
           ui.mailEdit->text().isEmpty()   ||
           ui.mdpEdit->text().isEmpty();
}

/* Retourne le message d'erreur correspondant au premier champ vide trouvé */
QString InscriptionForm::errorMessage()
{
    if (ui.nomEdit->text().isEmpty())    return "Le nom est obligatoire.";
    if (ui.prenomEdit->text().isEmpty()) return "Le prénom est obligatoire.";
    if (!ui.dateEdit->date().isValid())  return "La date de naissance est invalide.";
    if (ui.mailEdit->text().isEmpty())   return "L'adresse e-mail est obligatoire.";
    if (ui.mdpEdit->text().isEmpty())    return "Le mot de passe est obligatoire.";
    return "Erreur de formulaire.";
}

/* Remet tous les champs à leur état initial */
void InscriptionForm::clearAllInputs()
{
    ui.nomEdit->clear();
    ui.prenomEdit->clear();
    ui.mailEdit->clear();
    ui.mdpEdit->clear();
    ui.dateEdit->setDate(QDate::currentDate());
}

/* Enregistre le participant dans la base de données locale.
 * Le mot de passe est haché en SHA-256 avant stockage. */
void InscriptionForm::Register()
{
    const QString mdpHash = QString::fromUtf8(
        QCryptographicHash::hash(ui.mdpEdit->text().toLocal8Bit(),
                                 QCryptographicHash::Sha256).toHex());

    m_db->addParticipantRace(
        ui.nomEdit->text().trimmed(),
        ui.prenomEdit->text().trimmed(),
        ui.mailEdit->text().trimmed(),
        mdpHash,
        ui.dateEdit->text(),
        getSexe()
    );

    QMessageBox::information(this, "Inscription réussie",
        "Le participant " + ui.nomEdit->text().trimmed() +
        " " + ui.prenomEdit->text().trimmed() + " est désormais inscrit !");

    clearAllInputs();
    close();
}

/* Retourne l'identifiant genre (1=masculin, 2=féminin, 3=non binaire)
 * selon la sélection dans le combobox */
int InscriptionForm::getSexe()
{
    switch (ui.comboBox->currentIndex()) {
    case 0:  return 1;
    case 1:  return 2;
    case 2:  return 3;
    default: return 1;
    }
}

/* Validation et enregistrement à la soumission du formulaire */
void InscriptionForm::on_inscriptionButton_clicked()
{
    if (isFormEmpty()) {
        QMessageBox::critical(this, "Formulaire incomplet", errorMessage());
        return;
    }

    if (m_db->isParticipantExist(ui.mailEdit->text().trimmed())) {
        QMessageBox::critical(this, "Inscription",
            "Un participant possède déjà cette adresse e-mail.");
        return;
    }

    Register();
}
