#include "application.h"
#include "ui_application.h"
#include "racemanager.h"
#include "appconfig.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>

/* ──────────────────────────────────────────────────────────────────────────
 *  Initialisation complète
 * ────────────────────────────────────────────────────────────────────────── */
void Application::init()
{
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(this,
                              "Pilote SQLite manquant",
                              "Le pilote QSQLITE n'a pas été trouvé.");
        return;
    }

    m_db = new DatabaseManager();
    QSqlError err = m_db->initDb();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

    configuration   = new Configuration();
    m_sqlite        = new SQLiteConverter("course.db");
    gestion_participant = new GestionParticipant(m_db);
    inscription_form    = new InscriptionForm(m_db);
    config_form         = new ConfigForm();
    gestion_portique    = new GestionPortique();

    /* Récupérer les paramètres MySQL depuis la config (QJsonValueRef évité) */
    QJsonObject dbCfg = config_form->getDatabaseData();

    if (dbCfg.contains("ip")           && dbCfg.contains("port") &&
        dbCfg.contains("user")         && dbCfg.contains("mot_de_passe") &&
        dbCfg.contains("database")     &&
        !dbCfg["database"].toString().isEmpty())
    {
        m_mydb = new MySQLData(dbCfg["user"].toString(),
                               dbCfg["mot_de_passe"].toString(),
                               dbCfg["ip"].toString(),
                               dbCfg["database"].toString(),
                               this);
    }
    else {
        /* Fallback : variables d'environnement via AppConfig */
        AppConfig &cfg = AppConfig::instance();
        m_mydb = new MySQLData(cfg.dbUser(), cfg.dbPassword(),
                               cfg.dbHost(), cfg.dbName(), this);
    }

    config_form->init(m_mydb);

    /* Connexions aux signaux asynchrones */
    connect(m_mydb, &MySQLData::importFinished,
            this,   &Application::onImportFinished);
    connect(m_mydb, &MySQLData::exportFinished,
            this,   &Application::onExportFinished);

    initRaces();
}

Application::Application(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Application)
{
    ui->setupUi(this);
    setWindowTitle("CourseOrientation");
    init();
}

Application::~Application()
{
    delete ui;
    delete gestion_participant;
    delete inscription_form;
    delete config_form;
    delete gestion_portique;
    delete configuration;
    delete m_sqlite;
    /* m_mydb est un enfant Qt (parent=this), détruit automatiquement */
    delete m_db;
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Remplissage de la comboBox des courses
 * ────────────────────────────────────────────────────────────────────────── */
void Application::initRaces()
{
    QSqlQuery query(m_db->getDb());
    query.prepare("SELECT * FROM races");
    if (!query.exec())
        return;

    QVector<RaceManager::Race> races;
    while (query.next()) {
        RaceManager::Race race;
        race.raceId       = query.value(0).toInt();
        race.departmentId = query.value(1).toInt();
        race.name         = query.value(2).toString();
        race.date         = query.value(3).toString();
        race.location     = query.value(4).toString();
        race.gpsLongitude = query.value(5).toFloat();
        race.gpsLatitude  = query.value(6).toFloat();
        race.difficulty   = query.value(7).toInt();
        race.type         = query.value(8).toInt();
        race.book         = query.value(9).toInt();
        races.push_back(race);
    }

    RaceManager::getInstance()->setRaces(races);

    ui->comboBox->clear();
    ui->comboBox->addItem("-");
    comboLists.clear();
    comboLists.append("-");

    for (const RaceManager::Race &r : std::as_const(races)) {
        ui->comboBox->addItem(
            QDateTime::fromString(r.date, Qt::ISODate)
                .toLocalTime()
                .toString("yyyy/MM/dd hh:mm:ss")
            + "  " + r.name);
        comboLists.append(r.date);
    }
}

void Application::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Erreur base de données", err.text());
}

/* Active ou désactive les actions pendant les opérations réseau */
void Application::setOperationsEnabled(bool enabled)
{
    ui->actionImporter->setEnabled(enabled);
    ui->actionExporter->setEnabled(enabled);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Slots des résultats asynchrones
 * ────────────────────────────────────────────────────────────────────────── */
void Application::onImportFinished(bool success, const QString &message)
{
    setOperationsEnabled(true);
    if (success) {
        initRaces();
        QMessageBox::information(this, "Importation", message);
    } else {
        QMessageBox::warning(this, "Importation",
                             "Erreur lors de l'importation :\n" + message);
    }
}

void Application::onExportFinished(bool success, const QString &message)
{
    setOperationsEnabled(true);
    if (success)
        QMessageBox::information(this, "Exportation", message);
    else
        QMessageBox::warning(this, "Exportation",
                             "Erreur lors de l'exportation :\n" + message);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Actions du menu
 * ────────────────────────────────────────────────────────────────────────── */
void Application::on_actionConfiguration_triggered() {}

void Application::on_actionInscription_triggered()
{
    if (RaceManager::getInstance()->isRaceSelected())
        inscription_form->show();
    else
        QMessageBox::warning(this, "Inscription",
                             "Sélectionnez une course avant d'accéder à cette page.");
}

void Application::on_actionGestion_des_participants_triggered()
{
    gestion_participant->createTableView();
    gestion_participant->show();
}

void Application::on_actionBddConfig_triggered()
{
    config_form->show();
}

void Application::on_actionExporter_triggered()
{
    if (!RaceManager::getInstance()->isRaceSelected()) {
        QMessageBox::warning(this, "Export",
                             "Sélectionnez une course avant d'exporter.");
        return;
    }
    setOperationsEnabled(false);
    m_mydb->exportDataAsync();
}

void Application::on_actionImporter_triggered()
{
    setOperationsEnabled(false);
    m_mydb->importDataAsync();
}

void Application::on_buttonSelectRace_clicked()
{
    const int idx = ui->comboBox->currentIndex();
    if (idx <= 0 || idx >= comboLists.size()) return;

    const QString dateChoisie = comboLists.at(idx);

    for (const RaceManager::Race &race : RaceManager::getInstance()->getRaces()) {
        if (race.date == dateChoisie) {
            RaceManager::getInstance()->setRaceSelected(true);
            RaceManager::getInstance()->setRace(
                race.raceId, race.departmentId, race.name, race.date,
                race.location, race.gpsLongitude, race.gpsLatitude,
                race.difficulty, race.type, race.book);

            QMessageBox::information(this, "Course",
                                     "Course sélectionnée : " + race.name);

            for (QAction *ac : ui->menuParam_tre->actions())
                ac->setEnabled(true);
            for (QAction *ac : ui->menuFen_tre->actions())
                ac->setEnabled(true);
            return;
        }
    }
}

void Application::on_actionPortique_triggered()
{
    if (RaceManager::getInstance()->isRaceSelected())
        gestion_portique->show();
    else
        QMessageBox::warning(this, "Portique",
                             "Sélectionnez une course avant d'accéder à cette page.");
}

void Application::on_actionQuitter_2_triggered() { close(); }

void Application::on_actionReinitialiser_les_donn_es_triggered()
{
    if (!RaceManager::getInstance()->isRaceSelected()) {
        QMessageBox::warning(this, "Réinitialisation",
                             "Sélectionnez une course avant de réinitialiser.");
        return;
    }

    const int raceId = RaceManager::getInstance()->getRaceId();

    QSqlQuery q(m_db->getDb());
    q.prepare("DELETE FROM participant_races WHERE race_id=?");
    q.addBindValue(raceId);
    q.exec();

    q.prepare("DELETE FROM participant_races_data WHERE race_id=?");
    q.addBindValue(raceId);
    q.exec();

    q.prepare("DELETE FROM checkpoints WHERE race_id=?");
    q.addBindValue(raceId);
    q.exec();

    QMessageBox::information(this, "Réinitialisation",
                             "Données de la course réinitialisées avec succès.");
}
