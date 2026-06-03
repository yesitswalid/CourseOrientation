#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QSqlError>
#include "configuration.h"
#include "gestionparticipant.h"
#include "gestionportique.h"
#include "inscriptionform.h"
#include "databasemanager.h"
#include "configform.h"
#include "SQLiteConverter.h"
#include "mysqldata.h"
#include "racemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Application; }
QT_END_NAMESPACE

/**
 * @class Application
 * @brief Fenêtre principale de l'application.
 *
 * Coordonne toutes les sous-fenêtres, la base de données locale (SQLite)
 * et les opérations d'import/export asynchrones vers le serveur MySQL.
 */
class Application : public QMainWindow
{
    Q_OBJECT

public:
    Application(QWidget *parent = nullptr);
    ~Application() override;

    GestionParticipant *gestion_participant = nullptr;
    InscriptionForm    *inscription_form    = nullptr;
    GestionPortique    *gestion_portique    = nullptr;
    ConfigForm         *config_form         = nullptr;
    DatabaseManager    *m_db                = nullptr;
    MySQLData          *m_mydb              = nullptr;
    Configuration      *configuration       = nullptr;
    SQLiteConverter    *m_sqlite            = nullptr;

private slots:
    void on_actionConfiguration_triggered();
    void on_actionInscription_triggered();
    void on_actionGestion_des_participants_triggered();
    void on_actionBddConfig_triggered();
    void on_actionImporter_triggered();
    void on_buttonSelectRace_clicked();
    void on_actionPortique_triggered();
    void on_actionQuitter_2_triggered();
    void on_actionExporter_triggered();
    void on_actionReinitialiser_les_donn_es_triggered();

    /* Résultats des opérations asynchrones MySQL */
    void onImportFinished(bool success, const QString &message);
    void onExportFinished(bool success, const QString &message);

    /** Ouvre la boîte de dialogue de création d'une nouvelle course. */
    void onCreateRaceClicked();

private:
    Ui::Application   *ui;
    QVector<QString>   comboLists;

    void init();
    void initRaces();
    void showError(const QSqlError &err);
    void setOperationsEnabled(bool enabled);

    /** Ajoute le bouton "Nouvelle course" et l'action menu correspondante. */
    void setupCreateRaceButton();

    /** Vérifie la disponibilité du driver MySQL. Affiche un avertissement et
     *  désactive les actions Import/Export si QMYSQL n'est pas chargé. */
    void checkMySQLDriver();
};

#endif // APPLICATION_H
