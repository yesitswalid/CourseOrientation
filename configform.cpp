#include "configform.h"
#include "ui_configform.h"
#include <QMessageBox>
#include <QtSql>


#define PORT_DEFAULT 3306


ConfigForm::ConfigForm()
{
   ui.setupUi(this);
   this->configuration = new Configuration();

   QSqlDatabase m_dbCopy = QSqlDatabase::addDatabase("QMYSQL", "myTestMysql");
   m_db = new QSqlDatabase(m_dbCopy);
   m_dbCopy.close();

   //récuperation du contenus de la configuration
   QJsonObject obj = configuration->getAll();
   //chercher la clé et avoir les sous clées
   QJsonValueRef m_values = obj.find("base_de_donnees").value();
   //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
   QJsonObject m_obj_values = m_values.toObject();

   //config json par defaut
   QJsonObject m_obj_default;
   m_obj_default["ip"] = "127.0.0.1";
   m_obj_default["port"] = PORT_DEFAULT;
   m_obj_default["user"] = "root";
   m_obj_default["mot_de_passe"] = "";
   m_obj_default["database"] = "";

   //boucle permettant de vérifier chaque clée de la config
   for(int i=0; i<m_obj_default.size(); i++)
   {
       //Savoir si dans l'objet de la config il y a les clées et valeur de l'objet par default cas contraire l'objet de la config sera affecter par les clé et valeur manquant.
       if(!m_obj_values.contains(m_obj_default.keys().at(i))) {
           m_obj_values.insert(m_obj_default.keys().at(i), m_obj_default.value(m_obj_default.keys().at(i)));
       }
   }


   //Creation d'un nouveau object
   QJsonObject m_obj;
   //affectation de ip et port dans l'objet base_de_donnees
   m_obj["base_de_donnees"] = m_obj_values;
   //Affecter dans la configuration
   configuration->setAll(m_obj);

   //savoir si la configuration contient ip et le port de la bdd pour l'initialiser dans les textes
   if(m_obj_values.contains("ip") && m_obj_values.contains("port") && m_obj_values.contains("user") && m_obj_values.contains("mot_de_passe") && m_obj_values.contains("database"))
   {
       ui.ipEdit->setText(m_obj_values["ip"].toString());
       ui.portEdit->setText(QString::number(m_obj_values["port"].toInt()));
       ui.userEdit->setText(m_obj_values["user"].toString());
       ui.mdpEdit->setText(m_obj_values["mot_de_passe"].toString());
       ui.dbEdit->setText(m_obj_values["database"].toString());
   }
}

ConfigForm::~ConfigForm()
{
    //Nettoyer la memoire en enlevant la config.
    delete configuration;
    m_db->close();
    delete m_db;

}

void ConfigForm::on_continueButton_clicked()
{
    //Savoir si l'ip ou le port, le nom d'utilisateur, le mot de passe etc.. sont des champs vides cela retournera un Message box critical "Veuillez remplir le champs de la base donnéesé
    if(ui.ipEdit->text().isEmpty() || ui.portEdit->text().isEmpty() ||
            ui.userEdit->text().isEmpty() || ui.mdpEdit->text().isEmpty() ||
            ui.dbEdit->text().isEmpty())
    {
        QMessageBox::critical(this, "Base de données",
                    "Veuillez remplir le champs de la base de données");
        return;
    }

    QJsonObject obj = configuration->getAll();
    //chercher la clé et avoir les sous clées
    QJsonValueRef m_values = obj.find("base_de_donnees").value();

    QJsonObject m_obj_values = m_values.toObject();

    if(m_obj_values.contains("ip") && m_obj_values.contains("port") &&
            m_obj_values.contains("user") && m_obj_values.contains("mot_de_passe") &&
            m_obj_values.contains("database") &&
            ui.ipEdit->text().replace(" ", "") == m_obj_values["ip"].toString() &&
            ui.portEdit->text().replace(" ", "") == QString::number(m_obj_values["port"].toInt()) &&
            ui.userEdit->text().replace(" ", "") == m_obj_values["user"].toString() &&
            ui.mdpEdit->text().replace(" ", "") == m_obj_values["mot_de_passe"].toString() &&
            ui.dbEdit->text().replace(" ", "") == m_obj_values["database"].toString()
            )
    {

        return;//evite la re-écriture en boucle.
    }

    //inserer l'ip et le port dans l'objet
    m_obj_values.insert("ip", ui.ipEdit->text().replace(" ", ""));
    m_obj_values.insert("port", ui.portEdit->text().replace(" ", "").toInt());
    m_obj_values.insert("user", ui.userEdit->text().replace(" ", ""));
    m_obj_values.insert("mot_de_passe", ui.mdpEdit->text().replace(" ", ""));
    m_obj_values.insert("database", ui.dbEdit->text().replace(" ", ""));

    //Creation d'un nouveau object
    QJsonObject m_obj;
    //affectation de ip et port dans l'objet base_de_donnees
    m_obj["base_de_donnees"] = m_obj_values;
    //Affecter dans la configuration
    //inserer la nouvelle ip et le port dans la configuration
    configuration->setAll(m_obj);


    ui.ipEdit->setText(m_obj_values["ip"].toString());
    ui.portEdit->setText(QString::number(m_obj_values["port"].toInt()));
    ui.userEdit->setText(m_obj_values["user"].toString());
    ui.mdpEdit->setText(m_obj_values["mot_de_passe"].toString());
    ui.dbEdit->setText(m_obj_values["database"].toString());



    QMessageBox::information(this, "Base de données",
                             "Vous avez mis à jour la configuration de la base de données !");

}


void ConfigForm::on_testButton_clicked()
{
    if(ui.ipEdit->text().isEmpty() || ui.portEdit->text().isEmpty() ||
            ui.userEdit->text().isEmpty() || ui.mdpEdit->text().isEmpty() ||
            ui.dbEdit->text().isEmpty())
    {
        QMessageBox::critical(this, "Base de données",
                    "Veuillez remplir le champs de la base de données");
        return;
    }

    m_db->setUserName(ui.userEdit->text().replace(" ", ""));
    m_db->setPassword(ui.mdpEdit->text().replace(" ", ""));
    m_db->setHostName(ui.ipEdit->text().replace(" ", ""));
    m_db->setDatabaseName(ui.dbEdit->text().replace(" ", ""));
    m_db->setPort(ui.portEdit->text().toInt());

    if (m_db->open())
    {
        QMessageBox::information(this, "Base de données",
                    "Test Connection Ok. (Base de données connecté)");
    } else {
        QMessageBox::critical(this, "Base de données",
                    "Connexion echouée veuillez vérifiez vos informations.");
    }
}

QJsonObject ConfigForm::getDatabaseData()
{
    //récuperation du contenus de la configuration
    QJsonObject obj = this->configuration->getAll();
    //chercher la clé et avoir les sous clées
    QJsonValueRef m_values = obj.find("base_de_donnees").value();
    //transformer en objet pour permettre la récuperation des sous clée de la clé principal de "base_de_données"
    QJsonObject m_obj_values = m_values.toObject();

    return m_obj_values;
}

