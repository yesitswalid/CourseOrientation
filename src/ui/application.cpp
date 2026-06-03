#include "application.h"
#include "ui_application.h"
#include "racemanager.h"
#include "appconfig.h"
#include "createracedialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>
#include <QPushButton>
#include <QAction>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QSplitter>

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

    /* Le sélecteur de port série de GestionPortique reconfigure
     * automatiquement le QSerialPort de GestionParticipant */
    connect(gestion_portique, &GestionPortique::serialPortChanged,
            gestion_participant, &GestionParticipant::setSerialPort);

    /* Bouton et action menu pour créer une course */
    setupCreateRaceButton();

    /* Vérifier la disponibilité du driver MySQL pour l'import/export */
    checkMySQLDriver();

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

/* ──────────────────────────────────────────────────────────────────────────
 *  Ajoute le bouton "+ Nouvelle course" CÔTE À CÔTE avec le bouton "Valider"
 *  et centre le contenu principal pour qu'il reste lisible en plein écran.
 * ────────────────────────────────────────────────────────────────────────── */
void Application::setupCreateRaceButton()
{
    /* ── Bouton "+ Nouvelle course" ──────────────────────────────────────── */
    auto *boutonNouveau = new QPushButton("+ Nouvelle course");
    boutonNouveau->setObjectName("buttonCreateRace");
    boutonNouveau->setToolTip("Créer une nouvelle course (Ctrl+N)");
    boutonNouveau->setCursor(Qt::PointingHandCursor);
    boutonNouveau->setMinimumHeight(36);

    connect(boutonNouveau, &QPushButton::clicked,
            this, &Application::onCreateRaceClicked);

    /* ── Remplacer le bouton "Valider" par un conteneur horizontal ──────── */
    if (ui->buttonSelectRace) {
        QWidget *parent = ui->buttonSelectRace->parentWidget();
        if (parent) {
            ui->buttonSelectRace->setMinimumHeight(36);

            /* Conteneur horizontal pour les 2 boutons */
            auto *container = new QWidget();
            auto *hLayout = new QHBoxLayout(container);
            hLayout->setContentsMargins(0, 0, 0, 0);
            hLayout->setSpacing(10);
            hLayout->addWidget(ui->buttonSelectRace);
            hLayout->addWidget(boutonNouveau);

            /* Si le parent est un QSplitter, on insère à la même position */
            if (auto *splitter = qobject_cast<QSplitter *>(parent)) {
                const int idx = splitter->indexOf(ui->buttonSelectRace);
                splitter->insertWidget(idx, container);
            }
            /* Sinon, layout standard (QBoxLayout) */
            else if (auto *layout = qobject_cast<QBoxLayout *>(parent->layout())) {
                const int idx = layout->indexOf(ui->buttonSelectRace);
                layout->insertWidget(idx, container);
            }
        }
    }

    /* ── Centrer le contenu principal (max-width + spacers latéraux) ────── */
    if (auto *central = centralWidget()) {
        /* Le widget central actuel devient le contenu, on l'enveloppe dans
         * un nouveau wrapper centré avec une largeur maximale raisonnable. */
        const int CONTENU_LARGEUR_MAX = 720;

        /* Limiter la largeur du contenu interne */
        if (auto *splitter = central->findChild<QSplitter *>()) {
            splitter->setMaximumWidth(CONTENU_LARGEUR_MAX);
        }

        /* Wrapper le centralWidget avec un layout horizontal centré */
        auto *ancienLayout = central->layout();
        if (ancienLayout && ancienLayout->count() > 0) {
            ancienLayout->setContentsMargins(20, 20, 20, 20);
            ancienLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        }
    }

    /* ── Action dans la barre de menu Edition ───────────────────────────── */
    auto *actionCreer = new QAction(tr("&Créer une nouvelle course..."), this);
    actionCreer->setShortcut(QKeySequence("Ctrl+N"));
    actionCreer->setStatusTip(tr("Créer une course dans la base locale"));
    connect(actionCreer, &QAction::triggered,
            this, &Application::onCreateRaceClicked);

    if (ui->menuEdition)
        ui->menuEdition->addAction(actionCreer);
    else
        menuBar()->addAction(actionCreer);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Slot — Ouvre le dialogue de création de course
 * ────────────────────────────────────────────────────────────────────────── */
void Application::onCreateRaceClicked()
{
    auto *dlg = new CreateRaceDialog(m_db, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    /* Quand une course est créée, rafraîchir la liste déroulante */
    connect(dlg, &CreateRaceDialog::raceCreated,
            this, &Application::initRaces);

    dlg->show();
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Vérifie si le driver QMYSQL est chargé. Si non, désactive les actions
 *  d'import/export et affiche un message informatif au démarrage.
 * ────────────────────────────────────────────────────────────────────────── */
void Application::checkMySQLDriver()
{
    if (QSqlDatabase::drivers().contains("QMYSQL"))
        return; /* Driver disponible — rien à faire */

    /* Driver absent : désactiver Import/Export */
    if (ui->actionImporter) ui->actionImporter->setEnabled(false);
    if (ui->actionExporter) ui->actionExporter->setEnabled(false);

    const QString message = tr(
        "Le driver MySQL (QMYSQL) n'est pas disponible.\n\n"
        "Les fonctions d'import et d'export vers le serveur MySQL "
        "sont désactivées.\n\n"
        "Vous pouvez néanmoins utiliser l'application normalement "
        "avec la base locale SQLite (création de courses, inscription "
        "des participants, suivi des courses).\n\n"
        "Pour activer la synchronisation MySQL, consultez le README "
        "(section « Installation du driver MySQL »).");

    QMessageBox::information(this, tr("Driver MySQL absent"), message);
}
