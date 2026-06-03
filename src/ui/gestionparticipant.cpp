#include "gestionparticipant.h"
#include "ui_gestionparticipant.h"
#include "racemanager.h"
#include "databasemanager.h"
#include "server.h"
#include <QtSql>
#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>

/* ──────────────────────────────────────────────────────────────────────────
 *  Initialisation commune aux deux constructeurs
 * ────────────────────────────────────────────────────────────────────────── */
/* Nom du port série selon la plateforme — surchargeable via le .pro */
#ifndef SERIAL_PORT_DEFAULT
#  ifdef Q_OS_WIN
#    define SERIAL_PORT_DEFAULT "COM3"
#  else
#    define SERIAL_PORT_DEFAULT "/dev/ttyUSB0"
#  endif
#endif

static void configurerPortSerie(QSerialPort *serial)
{
    serial->setPortName(QString::fromLatin1(SERIAL_PORT_DEFAULT));

    if (!serial->setBaudRate(QSerialPort::Baud9600))
        qWarning() << "Erreur : impossible de configurer le débit à 9600 bauds";
    if (!serial->setStopBits(QSerialPort::OneStop))
        qWarning() << "Erreur : impossible de configurer le bit d'arrêt à 1";
    if (!serial->setDataBits(QSerialPort::Data8))
        qWarning() << "Erreur : impossible de configurer le format de données à 8 bits";
    if (!serial->setFlowControl(QSerialPort::NoFlowControl))
        qWarning() << "Erreur : impossible de désactiver le contrôle de flux";
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Constructeur sans base de données (crée sa propre instance DatabaseManager)
 * ────────────────────────────────────────────────────────────────────────── */
GestionParticipant::GestionParticipant()
{
    m_db = new DatabaseManager();
    ui.setupUi(this);

    /* Réception de l'identifiant badge RFID en mode RAZ */
    connect(Server::getInstance(), SIGNAL(getCardId(QString)),
            this, SLOT(getCardId(QString)));

    /* Réception des données de passage (points, balises) en mode DATA */
    connect(Server::getInstance(), SIGNAL(getResultDataParticipant(QString, int, int)),
            this, SLOT(getResultDataParticipant(QString, int, int)));

    createMenuBar();
    sim_config = new Configuration("simulation.json");

    /* Initialisation du port série pour la lecture du portique physique */
    serial = new QSerialPort();
    configurerPortSerie(serial);

    if (serial->open(QIODevice::ReadOnly))
        connect(serial, &QSerialPort::readyRead, this, &GestionParticipant::serialReceived);
    else
        qWarning() << "Erreur ouverture port série :" << serial->errorString();
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Constructeur principal — reçoit la base de données depuis Application
 * ────────────────────────────────────────────────────────────────────────── */
GestionParticipant::GestionParticipant(DatabaseManager *db)
{
    m_db = db;
    ui.setupUi(this);

    /* Réception de l'identifiant badge RFID en mode RAZ */
    connect(Server::getInstance(), SIGNAL(getCardId(QString)),
            this, SLOT(getCardId(QString)));

    /* Réception des données de passage (points, balises) en mode DATA */
    connect(Server::getInstance(), SIGNAL(getResultDataParticipant(QString, int, int)),
            this, SLOT(getResultDataParticipant(QString, int, int)));

    createMenuBar();
    sim_config = new Configuration("simulation.json");

    /* Initialisation du port série pour la lecture du portique physique */
    serial = new QSerialPort();
    configurerPortSerie(serial);

    if (serial->open(QIODevice::ReadOnly))
        connect(serial, &QSerialPort::readyRead, this, &GestionParticipant::serialReceived);
    else
        qWarning() << "Erreur ouverture port série :" << serial->errorString();
}

GestionParticipant::~GestionParticipant() {}

/* ──────────────────────────────────────────────────────────────────────────
 *  Affichage du tableau des participants inscrits à la course sélectionnée
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::createTableView()
{
    model = new QSqlRelationalTableModel(ui.participantTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("participants");

    /* Récupérer uniquement les participants inscrits à la course en cours */
    QSqlQuery q(m_db->getDb());
    q.prepare("SELECT p.id FROM participants AS p "
              "JOIN participant_races AS pr ON p.id = pr.participant_id "
              "WHERE pr.race_id = ?");
    q.addBindValue(RaceManager::getInstance()->getRaceId());

    if (q.exec()) {
        QString ids = "(";
        while (q.next())
            ids += q.value(0).toString() + ",";

        /* Refermer le statement SQL (remplacer la dernière virgule par une parenthèse) */
        ids = ids.left(ids.lastIndexOf(',')).append(")");
        model->setFilter("participants.id IN " + ids);
    }

    genreIdx = model->fieldIndex("genre_id");
    model->setRelation(genreIdx, QSqlRelation("genders", "id", "sexe"));

    model->setHeaderData(model->fieldIndex("lastname"),  Qt::Horizontal, tr("Nom"));
    model->setHeaderData(model->fieldIndex("firstname"), Qt::Horizontal, tr("Prénom"));
    model->setHeaderData(model->fieldIndex("mail"),      Qt::Horizontal, tr("Email"));
    model->setHeaderData(model->fieldIndex("password"),  Qt::Horizontal, tr("Mot de passe"));
    model->setHeaderData(model->fieldIndex("year"),      Qt::Horizontal, tr("Date de naissance"));
    model->setHeaderData(genreIdx,                       Qt::Horizontal, tr("Genre"));

    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    ui.participantTable->setModel(model);
    ui.participantTable->setItemDelegate(new QSqlRelationalDelegate(ui.participantTable));
    ui.participantTable->setColumnHidden(model->fieldIndex("id"), true);
    ui.participantTable->setSelectionMode(QAbstractItemView::SingleSelection);

    /* Liaison genre avec le combobox de sélection */
    ui.genreEdit->setModel(model->relationModel(genreIdx));
    ui.genreEdit->setModelColumn(model->relationModel(genreIdx)->fieldIndex("sexe"));

    /* Mappage des champs du modèle vers les widgets d'édition */
    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(ui.participantTable->model());
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui.nomEdit,    model->fieldIndex("lastname"));
    mapper->addMapping(ui.prenomEdit, model->fieldIndex("firstname"));
    mapper->addMapping(ui.mailEdit,   model->fieldIndex("mail"));
    mapper->addMapping(ui.genreEdit,  genreIdx);
    mapper->addMapping(ui.dateEdit,   model->fieldIndex("year"));

    connect(ui.participantTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper, &QDataWidgetMapper::setCurrentModelIndex);

    ui.participantTable->setCurrentIndex(model->index(0, 0));
}

void GestionParticipant::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quitter"), this);
    connect(quitAction, &QAction::triggered, this, &GestionParticipant::close);
}

void GestionParticipant::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Erreur base de données", "Détail : " + err.text());
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Suppression du participant sélectionné
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::on_suprButton_clicked()
{
    QModelIndexList selection = ui.participantTable->selectionModel()->selectedIndexes();
    if (selection.isEmpty()) return;

    m_db->removeParticipant(selection.at(0).data().value<int>());
    createTableView();
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Mise à jour des informations du participant sélectionné
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::on_updateButton_clicked()
{
    QModelIndexList sel = ui.participantTable->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) return;

    QSqlQuery q;
    q.prepare("UPDATE participants SET "
              "lastname=:nom, firstname=:prenom, mail=:mail, "
              "password=:mdp, year=:annee, genre_id=:genre "
              "WHERE id=:id");

    q.bindValue(":id",     sel.at(0).data().value<int>());
    q.bindValue(":nom",    sel.at(1).data().value<QString>());
    q.bindValue(":prenom", sel.at(2).data().value<QString>());
    q.bindValue(":mail",   sel.at(3).data().value<QString>());
    q.bindValue(":mdp",    sel.at(4).data().value<QString>());
    q.bindValue(":annee",  sel.at(5).data().value<QString>());

    const QString genre = sel.at(6).data().value<QString>();
    if      (genre == "masculin")   q.bindValue(":genre", 1);
    else if (genre == "feminin")    q.bindValue(":genre", 2);
    else                            q.bindValue(":genre", 3);

    if (q.exec()) {
        QMessageBox::information(this, "Participants",
            "Le participant " + sel.at(1).data().value<QString>() +
            " " + sel.at(2).data().value<QString>() + " a été mis à jour.");
        createTableView();
    } else {
        QMessageBox::warning(this, "Participants", "Erreur lors de la mise à jour.");
    }
}

/* ──────────────────────────────────────────────────────────────────────────
 *  RÉCEPTION BADGE RFID (mode RAZ)
 *
 *  Appelé par le signal Server::getCardId quand le lecteur UDP envoie
 *  un identifiant de badge RFID. Associe le badge au participant sélectionné.
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::getCardId(QString cardId)
{
    QModelIndexList sel = ui.participantTable->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) {
        QMessageBox::warning(this, "Lecteur", "Sélectionnez un participant avant de scanner le badge.");
        return;
    }

    if (RaceManager::getInstance()->getMode() != RaceManager::RAZ)
        return;

    /* Le bouton doigt (badge) doit être désactivé pour indiquer une capture en attente */
    if (ui.doigtButton->isEnabled())
        return;

    const int participantId = sel.at(0).data().value<int>();

    qDebug() << "==== BADGE RFID REÇU ====";
    qDebug() << "Mode       : RAZ";
    qDebug() << "Badge ID   :" << cardId;
    qDebug() << "Participant:" << participantId;
    qDebug() << "=========================";

    m_db->setFinger(participantId, cardId);
    ui.doigtButton->setEnabled(true);

    /* Afficher le nom du participant associé pour confirmation */
    QSqlQuery q(m_db->getDb());
    q.prepare("SELECT firstname, lastname FROM participants WHERE id=?");
    q.addBindValue(participantId);
    if (q.exec() && q.next()) {
        QMessageBox::information(this, "Lecteur",
            "Badge " + cardId + " associé à " +
            q.value(0).toString() + " " + q.value(1).toString());
    }
}

/* ──────────────────────────────────────────────────────────────────────────
 *  RÉCEPTION DONNÉES COURSE (mode DATA)
 *
 *  Appelé par le signal Server::getResultDataParticipant quand le lecteur UDP
 *  envoie les résultats d'un coureur (total points + nombre de balises validées).
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::getResultDataParticipant(QString carteId, int pointsTotal, int nbBalises)
{
    if (!RaceManager::getInstance()->isRaceSelected()) {
        QMessageBox::warning(this, "Lecteur",
            "Impossible de traiter les données : aucune course sélectionnée.");
        return;
    }

    QModelIndexList sel = ui.participantTable->selectionModel()->selectedIndexes();
    if (sel.isEmpty() || ui.doigtButton->isEnabled())
        return;

    const int participantId = sel.at(0).data().value<int>();

    qDebug() << "==== DONNÉES COURSE REÇUES ====";
    qDebug() << "Mode       : DATA";
    qDebug() << "Badge ID   :" << carteId;
    qDebug() << "Points     :" << pointsTotal;
    qDebug() << "Balises    :" << nbBalises;
    qDebug() << "Participant:" << participantId;
    qDebug() << "===============================";

    m_db->setPartipantPoints(participantId, pointsTotal);
    m_db->setPartipantBeacons(participantId, nbBalises);

    QSqlQuery q(m_db->getDb());
    q.prepare("SELECT firstname, lastname FROM participants WHERE id=?");
    q.addBindValue(participantId);
    if (q.exec() && q.next()) {
        QMessageBox::information(this, "Lecteur",
            "Données reçues pour " + q.value(0).toString() + " " + q.value(1).toString() + " !");
    }

    ui.doigtButton->setEnabled(true);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  PORTIQUE PHYSIQUE — lecture série (départ / arrivée)
 *
 *  Le portique émet via port série un identifiant de dossard (BID).
 *  En mode RAZ  → heure de départ enregistrée
 *  En mode DATA → heure d'arrivée enregistrée
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::serialReceived()
{
    QModelIndexList sel = ui.participantTable->selectionModel()->selectedIndexes();
    const int mode = RaceManager::getInstance()->getMode();

    if (!RaceManager::getInstance()->isRaceSelected())
        return;

    while (serial->canReadLine()) {
        QByteArray data = serial->readLine();

        /* Extraire l'identifiant de dossard à partir du caractère 7 */
        QString bid = QString::fromUtf8(data.mid(7)).replace("\r\n", "");

        if (mode == RaceManager::RAZ && !ui.portiqueButton->isEnabled()) {
            qDebug() << "[PORTIQUE / RAZ] Dossard :" << bid
                     << "Départ :" << QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");

            if (!sel.isEmpty()) {
                const int participantId = sel.at(0).data().value<int>();
                m_db->setPortiqueBID(participantId, bid);
                m_db->setDepartTimeParticipant(participantId, QDateTime::currentDateTime());
                QMessageBox::information(this, "Portique", "Départ du dossard #" + bid);
            } else {
                QMessageBox::warning(this, "Portique", "Sélectionnez un participant d'abord.");
            }
            ui.portiqueButton->setEnabled(true);

        } else if (mode == RaceManager::DATA && !ui.portiqueButton->isEnabled()) {
            qDebug() << "[PORTIQUE / DATA] Dossard :" << bid
                     << "Arrivée :" << QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");

            if (!sel.isEmpty()) {
                const int participantId = sel.at(0).data().value<int>();
                m_db->setPortiqueBID(participantId, bid);
                m_db->setFinishTimeParticipant(participantId, QDateTime::currentDateTime());
                QMessageBox::information(this, "Portique", "Arrivée du dossard #" + bid);
            } else {
                QMessageBox::warning(this, "Portique", "Sélectionnez un participant d'abord.");
            }
            ui.portiqueButton->setEnabled(true);
        }
    }
}

void GestionParticipant::on_portiqueButton_clicked()
{
    if (ui.participantTable->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox::warning(this, "Portique", "Sélectionnez un participant.");
        return;
    }
    ui.portiqueButton->setEnabled(false);
}

void GestionParticipant::on_doigtButton_clicked()
{
    if (ui.participantTable->selectionModel()->selectedIndexes().isEmpty()) {
        QMessageBox::warning(this, "Lecteur", "Sélectionnez un participant.");
        return;
    }
    ui.doigtButton->setEnabled(false);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  SIMULATION — Mode RAZ
 *
 *  Simule l'attribution d'un dossard (bid) et d'un badge RFID (finger)
 *  à un participant à partir des données du fichier simulation.json.
 *
 *  Structure attendue dans simulation.json :
 *  {
 *    "1": {
 *      "finger_id": <long>,          // identifiant du badge RFID
 *      "bid_id":    <string/int>,    // numéro de dossard
 *      "bid_start": <timestamp>,     // heure de départ (UNIX)
 *      "bid_end":   <timestamp>,     // heure d'arrivée (UNIX)
 *      "data": {                     // passages aux balises
 *        "1": { "longitude": ..., "latitude": ..., "altitude": ..., "order_id": ..., "points": ... }
 *      }
 *    }
 *  }
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::on_razSimButton_clicked()
{
    QModelIndexList sel = ui.participantTable->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) {
        QMessageBox::warning(this, "Simulation", "Sélectionnez un participant.");
        return;
    }

    RaceManager::getInstance()->setMode(RaceManager::RAZ);
    const int participantId = sel.at(0).data().value<int>();

    /* Vérifier que ce participant n'a pas déjà un dossard et un badge enregistrés */
    if (m_db->isFingerExist(participantId) && m_db->isPortiqueBIDExist(participantId)) {
        QMessageBox::warning(this, "Simulation RAZ",
            "Ce participant possède déjà un dossard et un badge RFID enregistrés.");
        return;
    }

    /* Charger l'entrée de simulation correspondant à ce tour de boucle */
    QJsonObject simData = sim_config->getAll();
    QJsonObject entreeParticipant = simData.value(QString::number(participantSimId++)).toObject();

    if (entreeParticipant.isEmpty()) {
        QMessageBox::warning(this, "Simulation", "Plus de données de simulation disponibles.");
        return;
    }

    const QString fingerId = entreeParticipant.value("finger_id").toVariant().toString();
    const QString bidId    = entreeParticipant.value("bid_id").toVariant().toString();

    m_db->setFinger(participantId, fingerId);
    m_db->setPortiqueBID(participantId, bidId);

    /* Confirmer le départ avec le numéro de dossard enregistré */
    QSqlQuery q(m_db->getDb());
    q.prepare("SELECT bid FROM participant_races WHERE participant_id=?");
    q.addBindValue(participantId);
    if (q.exec() && q.next())
        QMessageBox::information(this, "Départ",
            "Dossard #" + q.value(0).toString() + " a bien démarré la course.");
}

/* ──────────────────────────────────────────────────────────────────────────
 *  SIMULATION — Mode DATA
 *
 *  Simule la réception des données de passage d'un coureur depuis le lecteur.
 *  Identifie le participant par son badge RFID, enregistre ses passages aux
 *  balises et calcule la durée de la course.
 * ────────────────────────────────────────────────────────────────────────── */
void GestionParticipant::on_dataSimButton_clicked()
{
    QModelIndexList sel = ui.participantTable->selectionModel()->selectedIndexes();
    if (sel.isEmpty()) {
        QMessageBox::warning(this, "Simulation", "Sélectionnez un participant.");
        return;
    }

    RaceManager::getInstance()->setMode(RaceManager::DATA);
    const int participantId = sel.at(0).data().value<int>();

    if (!m_db->isFingerExist(participantId) || !m_db->isPortiqueBIDExist(participantId)) {
        QMessageBox::warning(this, "Simulation DATA",
            "Ce participant n'a pas encore de badge RFID ni de dossard (lancez d'abord le mode RAZ).");
        return;
    }

    /* Récupérer le badge RFID enregistré pour ce participant */
    QSqlQuery q(m_db->getDb());
    q.prepare("SELECT finger FROM participant_races WHERE participant_id=?");
    q.addBindValue(participantId);
    if (!q.exec() || !q.next()) return;
    const qlonglong fingerIdBdd = q.value(0).toLongLong();

    QJsonObject simData = sim_config->getAll();
    const int courseId = RaceManager::getInstance()->getRaceId();

    /* Chercher dans le JSON l'entrée dont le finger_id correspond à celui du participant */
    for (const QString &cle : simData.keys()) {
        QJsonObject entree = simData.value(cle).toObject();

        if (entree.value("finger_id").toVariant().toLongLong() != fingerIdBdd)
            continue;

        /* ── Enregistrement des passages aux balises ── */
        QJsonObject passages = entree.value("data").toObject();
        for (const QString &clePassage : passages.keys()) {
            QJsonObject passage = passages.value(clePassage).toObject();
            const int ordreId = passage.value("order_id").toInt();

            if (!m_db->hasParticipantCheckpoint(ordreId, courseId, participantId)) {
                /* Paramètres : altitude, longitude, latitude, race_id, participant_id, order_id, points */
                m_db->addParticipantCheckpoint(
                    passage.value("altitude").toString("0"),
                    passage.value("longitude").toString(),
                    passage.value("latitude").toString(),
                    courseId,
                    participantId,
                    ordreId,
                    passage.value("points").toInt()
                );
            }
        }

        /* ── Calcul de la durée de la course ── */
        const qint64 depart  = entree.value("bid_start").toVariant().toLongLong();
        const qint64 arrivee = entree.value("bid_end").toVariant().toLongLong();
        const float  dureeMinutes = (arrivee - depart) / 60.0f;

        /* Afficher le résultat avec le numéro de dossard */
        QSqlQuery qBid(m_db->getDb());
        qBid.prepare("SELECT bid FROM participant_races WHERE participant_id=?");
        qBid.addBindValue(participantId);
        if (qBid.exec() && qBid.next()) {
            QMessageBox::information(this, "Arrivée",
                "Dossard #" + qBid.value(0).toString() +
                " — Durée : " + QString::number(dureeMinutes, 'f', 1) + " min");
        }

        break; /* Un seul participant correspond */
    }
}

int GestionParticipant::getNewParticipantId()
{
    /* Non implémenté — réservé pour usage futur */
    return -1;
}
