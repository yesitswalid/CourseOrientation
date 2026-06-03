#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>
#include <QDate>
#include <QString>
#include <QCryptographicHash>

/* ──────────────────────────────────────────────────────────────────────────
 *  Schéma de la base de données SQLite locale
 * ────────────────────────────────────────────────────────────────────────── */

const auto PARTICIPANTS_SQL = QString(R"(
    CREATE TABLE IF NOT EXISTS participants(
        id          INTEGER PRIMARY KEY,
        lastname    VARCHAR,
        firstname   VARCHAR,
        mail        VARCHAR,
        password    VARCHAR,
        year        VARCHAR,
        genre_id    INTEGER)
)");

const auto PARTICIPANTS_RACES_SQL = QString(R"(
    CREATE TABLE IF NOT EXISTS participant_races(
        id              INTEGER PRIMARY KEY,
        participant_id  INTEGER,
        race_id         INTEGER,
        finger          VARCHAR DEFAULT NULL,
        bid             VARCHAR DEFAULT NULL)
)");

/* Données de résultat d'un participant à une course (points, balises, temps) */
const auto PARTICIPANTS_RACES_DATA_SQL = QString(R"(
    CREATE TABLE IF NOT EXISTS participant_races_data(
        race_id         INTEGER PRIMARY KEY,
        participant_id  INTEGER,
        beacons         INTEGER  DEFAULT 0,
        points          INTEGER  DEFAULT 0,
        start           DATETIME DEFAULT 0,
        end             DATETIME DEFAULT 0)
)");

const auto GENDERS_SQL = QString(R"(
    CREATE TABLE IF NOT EXISTS genders(
        id      INTEGER PRIMARY KEY,
        sexe    VARCHAR)
)");

const auto RACES_SQL = QString(R"(
    CREATE TABLE IF NOT EXISTS races(
        id              INTEGER PRIMARY KEY,
        id_department   INTEGER,
        name            VARCHAR,
        date            DATETIME DEFAULT NULL,
        location        VARCHAR  DEFAULT NULL,
        gps_longitude   VARCHAR,
        gps_latitude    VARCHAR,
        difficulty      INTEGER,
        type            INTEGER,
        book            INTEGER)
)");

/* Passage d'un participant à une balise GPS.
 * Note : la colonne "attitude" correspond en réalité à la latitude GPS
 * (typo conservée pour compatibilité avec la base existante). */
const auto CHECKPOINTS_SQL = QString(R"(
    CREATE TABLE IF NOT EXISTS checkpoints(
        id              INTEGER PRIMARY KEY,
        altitude        VARCHAR,
        longitude       VARCHAR,
        attitude        VARCHAR,
        race_id         INTEGER,
        participant_id  INTEGER,
        order_id        INTEGER,
        points          INTEGER)
)");

/* ── Requêtes INSERT ──────────────────────────────────────────────────────── */

const auto INSERT_PARTICIPANT_SQL = QString(R"(
    INSERT INTO participants(lastname, firstname, mail, password, year, genre_id)
    VALUES(?, ?, ?, ?, ?, ?)
)");

const auto INSERT_PARTICIPANT_RACE_SQL = QString(R"(
    INSERT INTO participant_races(participant_id, race_id)
    VALUES(?, ?)
)");

const auto INSERT_PARTICIPANT_DATA_RACE_SQL = QString(R"(
    INSERT INTO participant_races_data(race_id, participant_id, beacons, points)
    VALUES(?, ?, ?, ?)
)");

const auto INSERT_GENDER_SQL = QString(R"(
    INSERT OR REPLACE INTO genders(sexe) VALUES(?)
)");

const auto INSERT_RACE_SQL = QString(R"(
    INSERT INTO races(id, id_department, name, date, location,
                      gps_longitude, gps_latitude, difficulty, type, book)
    VALUES(?,?,?,?,?,?,?,?,?,?)
)");

const auto INSERT_CHECKPOINT_SQL = QString(R"(
    INSERT INTO checkpoints(altitude, longitude, attitude, race_id, participant_id, order_id, points)
    VALUES(?, ?, ?, ?, ?, ?, ?)
)");

/* ── Requêtes DELETE ─────────────────────────────────────────────────────── */

const auto DELETE_PARTICIPANT_SQL = QString(R"(
    DELETE FROM participants WHERE id = ?
)");

const auto DELETE_PARTICIPANT_RACE_SQL = QString(R"(
    DELETE FROM participant_races WHERE participant_id = ?
)");

const auto DELETE_PARTICIPANT_RACE_DATA_SQL = QString(R"(
    DELETE FROM participant_races_data WHERE participant_id = ?
)");

const auto DELETE_GENDER_SQL = QString(R"(
    DELETE FROM genders WHERE id = ?
)");

/* ── Requêtes DROP TABLE ─────────────────────────────────────────────────── */

const auto DROP_PARTICIPANT_SQL = QString(R"(DROP TABLE IF EXISTS participants)");
const auto DROP_GENDER_SQL      = QString(R"(DROP TABLE IF EXISTS genders)");

/* ── Requêtes SELECT ─────────────────────────────────────────────────────── */

const auto SELECT_GENRE_SEXE                      = QString(R"(SELECT * FROM genders WHERE sexe=?)");
const auto SELECT_DATA_PARTICIPANT                 = QString(R"(SELECT * FROM participants WHERE mail=?)");
const auto SELECT_DATA_EXIST_PARTICIPANT_RACE      = QString(R"(SELECT * FROM participant_races WHERE participant_id=?)");
const auto SELECT_DATA_PARTICIPANT_RACE            = QString(R"(SELECT * FROM participant_races WHERE race_id=?)");
const auto SELECT_DATA_RACE                        = QString(R"(SELECT * FROM races WHERE name=?)");
const auto SELECT_ORDER_CHECKPOINT_PARTICIPANT     = QString(R"(SELECT * FROM checkpoints WHERE order_id=? AND race_id=? AND participant_id=?)");
const auto SELECT_PARTICIPANT_CHECKPOINT           = QString(R"(SELECT * FROM checkpoints WHERE participant_id=? AND race_id=?)");

/* ──────────────────────────────────────────────────────────────────────────
 *  Classe DatabaseManager
 *
 *  Encapsule toutes les opérations sur la base SQLite locale (course.db).
 *  Chaque fenêtre de l'application reçoit un pointeur vers cette instance
 *  unique créée dans Application::init().
 * ────────────────────────────────────────────────────────────────────────── */
class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    /* Initialisation et connexion à la base SQLite locale */
    QSqlError initDb();
    QSqlError initDb(QString &connectionName);

    /** @brief Initialise une base SQLite en mémoire (:memory:) pour les tests unitaires.
     *  N'écrit aucun fichier sur le disque. */
    QSqlError initDbInMemory();

    /* Accès direct à l'objet base de données Qt */
    void         setDb(QSqlDatabase db);
    QSqlDatabase getDb() const;

    /* ── Participants ───────────────────────────────────────────────────── */
    void addParticipant(const QString &lastname, const QString &firstname,
                        const QString &mail, const QString &password,
                        const QString &year, int genreId);

    void addParticipantRace(const QString &lastname, const QString &firstname,
                            const QString &mail, const QString &password,
                            const QString &year, int genreId);
    void addParticipantRace(int participantId, int raceId);

    bool isParticipantExist(const QString &mail);
    bool isParticipantRaceExist(int participantId);
    void removeParticipant(int id);

    void insertGenreIfNotExist(const QString &sexe);

    QList<QVariant> getParticipantData(int participantId);

    /* ── Balises / checkpoints ──────────────────────────────────────────── */
    void addParticipantCheckpoint(const QString &altitude, const QString &longitude,
                                  const QString &latitude,
                                  int raceId, int participantId, int orderId, int points);
    bool hasParticipantCheckpoint(int orderId, int raceId, int participantId);

    /* ── Données de course (points, balises, temps) ─────────────────────── */
    void addParticipantRaceData(int participantId, int beacons, int points);
    bool hasParticipantRaceData(int participantId);

    void setDepartTimeParticipant(int participantId, QDateTime startTime);
    void setFinishTimeParticipant(int participantId, QDateTime endTime);
    void setPartipantBeacons(int participantId, int beacons);
    void setPartipantPoints(int participantId, int points);

    /* ── Dossard (BID) et badge RFID (doigt) ────────────────────────────── */
    bool isPortiqueBIDExist(int participantId);
    bool isFingerExist(int participantId);
    void setPortiqueBID(int participantId, const QString &bid);
    void setFinger(int participantId, const QString &fingerId);

    /* ── Courses ─────────────────────────────────────────────────────────── */
    void addRace(int raceId, int departmentId, const QString &name,
                 QDateTime date, const QString &location,
                 const QString &gpsLongitude, const QString &gpsLatitude,
                 int difficulty, int type, int book);
    bool isRaceExist(const QString &raceName);

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
