#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QSqlError>
#include <configuration.cpp>
#include <gestionparticipant.h>
#include <gestionportique.h>
#include <inscriptionform.h>
#include <databasemanager.h>
#include <configform.h>
#include <SQLiteConverter.cpp>
#include <mysqldata.cpp>
#include <racemanager.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Application; }
QT_END_NAMESPACE

class Application : public QMainWindow
{
    Q_OBJECT

public:
    Application(QWidget *parent = nullptr);
    ~Application();
    GestionParticipant *gestion_participant;
    InscriptionForm *inscription_form;
    GestionPortique *gestion_portique;
    ConfigForm *config_form;
    DatabaseManager *m_db;
    MySQLData *m_mydb;
    Configuration *configuration;
    SQLiteConverter *m_sqlite;
    RaceManager *getRace();

private slots:
    void on_actionConfiguration_triggered();

    void on_actionInscription_triggered();

    void on_actionGestion_des_participants_triggered();

    void on_actionBddConfig_triggered();

    void on_actionExporter_vers_triggered();

    void on_actionImporter_triggered();


    void on_buttonSelectRace_clicked();


    void on_actionPortique_triggered();

    void on_actionQuitter_2_triggered();

private:
    Ui::Application *ui;
    RaceManager *race;
    QVector <QString> comboLists;
    void init();
    void initRaces();
    void showError(const QSqlError &err);
};
#endif // APPLICATION_H
