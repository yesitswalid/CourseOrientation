#include "databasemanager.h"
#include "racemanager.h"
#include <QApplication>
#include <QDateTime>
#include <atomic>

/* ──────────────────────────────────────────────────────────────────────────
 *  Helpers internes
 * ────────────────────────────────────────────────────────────────────────── */

/** Crée un QSqlQuery associé à la connexion stockée dans m_db.
 *  Remplace l'ancienne notation m_db.exec() supprimée en Qt 6. */
static inline QSqlQuery makeQuery(const QSqlDatabase &db)
{
    return QSqlQuery(db);
}

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.commit();
        m_db.close();
    }
}

void DatabaseManager::setDb(QSqlDatabase db) { m_db = std::move(db); }
QSqlDatabase DatabaseManager::getDb() const  { return m_db; }

/* ──────────────────────────────────────────────────────────────────────────
 *  Initialisation — connexion SQLite locale par défaut
 * ────────────────────────────────────────────────────────────────────────── */
QSqlError DatabaseManager::initDb()
{
    /* Supprimer l'ancienne connexion par défaut si elle existe déjà */
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection))
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/course.db");

    if (!db.open())
        return db.lastError();

    setDb(db);

    {
        QSqlQuery q(db);
        if (!q.exec(PARTICIPANTS_SQL))           return q.lastError();
        if (!q.exec(GENDERS_SQL))                return q.lastError();
        if (!q.exec(RACES_SQL))                  return q.lastError();
        if (!q.exec(PARTICIPANTS_RACES_SQL))     return q.lastError();
        if (!q.exec(CHECKPOINTS_SQL))            return q.lastError();
        if (!q.exec(PARTICIPANTS_RACES_DATA_SQL)) return q.lastError();
    }

    insertGenreIfNotExist("masculin");
    insertGenreIfNotExist("feminin");
    insertGenreIfNotExist("Non binaire");

    return QSqlError();
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Initialisation avec nom de connexion personnalisé (import/export)
 * ────────────────────────────────────────────────────────────────────────── */
QSqlError DatabaseManager::initDb(QString &connectionName)
{
    if (QSqlDatabase::contains(connectionName))
        QSqlDatabase::removeDatabase(connectionName);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(QApplication::applicationDirPath() + "/course.db");

    if (!db.open())
        return db.lastError();

    setDb(db);

    {
        QSqlQuery q(db);
        if (!q.exec(PARTICIPANTS_SQL))           return q.lastError();
        if (!q.exec(GENDERS_SQL))                return q.lastError();
        if (!q.exec(RACES_SQL))                  return q.lastError();
        if (!q.exec(PARTICIPANTS_RACES_SQL))     return q.lastError();
        if (!q.exec(CHECKPOINTS_SQL))            return q.lastError();
        if (!q.exec(PARTICIPANTS_RACES_DATA_SQL)) return q.lastError();
    }

    insertGenreIfNotExist("masculin");
    insertGenreIfNotExist("feminin");
    insertGenreIfNotExist("Non binaire");

    return QSqlError();
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Initialisation en mémoire — pour les tests unitaires uniquement
 * ────────────────────────────────────────────────────────────────────────── */
QSqlError DatabaseManager::initDbInMemory()
{
    static std::atomic<int> compteur{0};
    const QString nomConnexion = QStringLiteral("co_test_%1").arg(compteur.fetch_add(1));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", nomConnexion);
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    setDb(db);

    {
        QSqlQuery q(db);
        if (!q.exec(PARTICIPANTS_SQL))           return q.lastError();
        if (!q.exec(GENDERS_SQL))                return q.lastError();
        if (!q.exec(RACES_SQL))                  return q.lastError();
        if (!q.exec(PARTICIPANTS_RACES_SQL))     return q.lastError();
        if (!q.exec(CHECKPOINTS_SQL))            return q.lastError();
        if (!q.exec(PARTICIPANTS_RACES_DATA_SQL)) return q.lastError();
    }

    insertGenreIfNotExist("masculin");
    insertGenreIfNotExist("feminin");
    insertGenreIfNotExist("Non binaire");

    return QSqlError();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  PARTICIPANTS
 * ══════════════════════════════════════════════════════════════════════════ */

bool DatabaseManager::isParticipantExist(const QString &mail)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(SELECT_DATA_PARTICIPANT);
    q.addBindValue(mail);
    return q.exec() && q.next();
}

bool DatabaseManager::isParticipantRaceExist(int participantId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(SELECT_DATA_EXIST_PARTICIPANT_RACE);
    q.addBindValue(participantId);
    return q.exec() && q.next();
}

void DatabaseManager::addParticipant(const QString &lastname, const QString &firstname,
                                     const QString &mail, const QString &password,
                                     const QString &year, int genreId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(INSERT_PARTICIPANT_SQL);
    q.addBindValue(lastname);
    q.addBindValue(firstname);
    q.addBindValue(mail);
    q.addBindValue(password);
    q.addBindValue(year);
    q.addBindValue(genreId);
    q.exec();
}

void DatabaseManager::addParticipantRace(const QString &lastname, const QString &firstname,
                                          const QString &mail, const QString &password,
                                          const QString &year, int genreId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(INSERT_PARTICIPANT_SQL);
    q.addBindValue(lastname);
    q.addBindValue(firstname);
    q.addBindValue(mail);
    q.addBindValue(password);
    q.addBindValue(year);
    q.addBindValue(genreId);
    q.exec();

    addParticipantRace(q.lastInsertId().toInt(), RaceManager::getInstance()->getRaceId());
}

void DatabaseManager::addParticipantRace(int participantId, int raceId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(INSERT_PARTICIPANT_RACE_SQL);
    q.addBindValue(participantId);
    q.addBindValue(raceId);
    q.exec();
}

QList<QVariant> DatabaseManager::getParticipantData(int participantId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare("SELECT * FROM participants WHERE id=?");
    q.addBindValue(participantId);

    QList<QVariant> data;
    if (q.exec() && q.next()) {
        for (int i = 0; i < q.record().count(); ++i)
            data.append(q.value(i));
    }
    return data;
}

void DatabaseManager::removeParticipant(int id)
{
    QSqlQuery q = makeQuery(m_db);

    q.prepare(DELETE_PARTICIPANT_SQL);
    q.addBindValue(id);
    q.exec();

    q.prepare(DELETE_PARTICIPANT_RACE_SQL);
    q.addBindValue(id);
    q.exec();

    q.prepare(DELETE_PARTICIPANT_RACE_DATA_SQL);
    q.addBindValue(id);
    q.exec();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  DONNÉES DE COURSE
 * ══════════════════════════════════════════════════════════════════════════ */

bool DatabaseManager::hasParticipantRaceData(int participantId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare("SELECT 1 FROM participant_races_data WHERE participant_id=? AND race_id=?");
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    return q.exec() && q.next();
}

void DatabaseManager::addParticipantRaceData(int participantId, int beacons, int points)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(INSERT_PARTICIPANT_DATA_RACE_SQL);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.addBindValue(beacons);
    q.addBindValue(points);
    q.exec();
}

void DatabaseManager::setPartipantPoints(int participantId, int points)
{
    if (!hasParticipantRaceData(participantId))
        addParticipantRaceData(participantId, 0, 0);

    QSqlQuery q = makeQuery(m_db);
    q.prepare("UPDATE participant_races_data SET points=? WHERE race_id=? AND participant_id=?");
    q.addBindValue(points);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}

void DatabaseManager::setPartipantBeacons(int participantId, int beacons)
{
    if (!hasParticipantRaceData(participantId))
        addParticipantRaceData(participantId, 0, 0);

    QSqlQuery q = makeQuery(m_db);
    q.prepare("UPDATE participant_races_data SET beacons=? WHERE race_id=? AND participant_id=?");
    q.addBindValue(beacons);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}

void DatabaseManager::setDepartTimeParticipant(int participantId, QDateTime startTime)
{
    if (!hasParticipantRaceData(participantId))
        addParticipantRaceData(participantId, 0, 0);

    QSqlQuery q = makeQuery(m_db);
    q.prepare("UPDATE participant_races_data SET start=? WHERE race_id=? AND participant_id=?");
    q.addBindValue(startTime);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}

void DatabaseManager::setFinishTimeParticipant(int participantId, QDateTime endTime)
{
    if (!hasParticipantRaceData(participantId))
        addParticipantRaceData(participantId, 0, 0);

    QSqlQuery q = makeQuery(m_db);
    q.prepare("UPDATE participant_races_data SET end=? WHERE race_id=? AND participant_id=?");
    q.addBindValue(endTime);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  BADGE RFID ET DOSSARD
 * ══════════════════════════════════════════════════════════════════════════ */

bool DatabaseManager::isFingerExist(int participantId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare("SELECT 1 FROM participant_races WHERE participant_id=? AND finger IS NOT NULL");
    q.addBindValue(participantId);
    return q.exec() && q.next();
}

void DatabaseManager::setFinger(int participantId, const QString &fingerId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare("UPDATE participant_races SET finger=? WHERE participant_id=? AND race_id=?");
    q.addBindValue(fingerId);
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.exec();
}

bool DatabaseManager::isPortiqueBIDExist(int participantId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare("SELECT 1 FROM participant_races WHERE participant_id=? AND race_id=? AND bid IS NOT NULL");
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    return q.exec() && q.next();
}

void DatabaseManager::setPortiqueBID(int participantId, const QString &bid)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare("UPDATE participant_races SET bid=? WHERE participant_id=? AND race_id=?");
    q.addBindValue(bid);
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.exec();
}

void DatabaseManager::insertGenreIfNotExist(const QString &sexe)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(SELECT_GENRE_SEXE);
    q.addBindValue(sexe);
    if (q.exec() && !q.next()) {
        q.prepare(INSERT_GENDER_SQL);
        q.addBindValue(sexe);
        q.exec();
    }
}

/* ══════════════════════════════════════════════════════════════════════════
 *  COURSES
 * ══════════════════════════════════════════════════════════════════════════ */

bool DatabaseManager::isRaceExist(const QString &raceName)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(SELECT_DATA_RACE);
    q.addBindValue(raceName);
    return q.exec() && q.next();
}

void DatabaseManager::addRace(int raceId, int departmentId, const QString &name,
                              QDateTime date, const QString &location,
                              const QString &gpsLongitude, const QString &gpsLatitude,
                              int difficulty, int type, int book)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(INSERT_RACE_SQL);
    q.addBindValue(raceId);
    q.addBindValue(departmentId);
    q.addBindValue(name);
    q.addBindValue(date);
    q.addBindValue(location);
    q.addBindValue(gpsLongitude);
    q.addBindValue(gpsLatitude);
    q.addBindValue(difficulty);
    q.addBindValue(type);
    q.addBindValue(book);
    q.exec();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  CHECKPOINTS / BALISES
 * ══════════════════════════════════════════════════════════════════════════ */

void DatabaseManager::addParticipantCheckpoint(const QString &altitude, const QString &longitude,
                                               const QString &latitude,
                                               int raceId, int participantId,
                                               int orderId, int points)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(INSERT_CHECKPOINT_SQL);
    q.addBindValue(altitude);
    q.addBindValue(longitude);
    q.addBindValue(latitude);
    q.addBindValue(raceId);
    q.addBindValue(participantId);
    q.addBindValue(orderId);
    q.addBindValue(points);
    q.exec();
}

bool DatabaseManager::hasParticipantCheckpoint(int orderId, int raceId, int participantId)
{
    QSqlQuery q = makeQuery(m_db);
    q.prepare(SELECT_ORDER_CHECKPOINT_PARTICIPANT);
    q.addBindValue(orderId);
    q.addBindValue(raceId);
    q.addBindValue(participantId);
    return q.exec() && q.next();
}
