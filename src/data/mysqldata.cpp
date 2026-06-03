#include "mysqldata.h"
#include "databasemanager.h"
#include "racemanager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <memory>
#include <atomic>

/* ──────────────────────────────────────────────────────────────────────────
 *  Génère un nom de connexion unique pour un thread du pool
 * ────────────────────────────────────────────────────────────────────────── */
QString MySQLData::threadConnName(const QString &prefix)
{
    static std::atomic<int> cpt{0};
    return QStringLiteral("%1_%2_%3")
        .arg(prefix)
        .arg(reinterpret_cast<quintptr>(QThread::currentThread()), 0, 16)
        .arg(cpt.fetch_add(1));
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Constructeurs / Destructeur
 * ────────────────────────────────────────────────────────────────────────── */

MySQLData::MySQLData(QObject *parent)
    : QObject(parent)
{}

MySQLData::MySQLData(const QString &username, const QString &password,
                     const QString &hostname, const QString &dbName,
                     QObject *parent)
    : QObject(parent)
    , m_params{username, password, hostname, dbName}
{
    m_db = new QSqlDatabase(
        QSqlDatabase::addDatabase("QMYSQL", threadConnName("main_mysql")));
    m_db->setUserName(username);
    m_db->setPassword(password);
    m_db->setHostName(hostname);
    m_db->setDatabaseName(dbName);
}

MySQLData::~MySQLData()
{
    if (m_db) {
        const QString connName = m_db->connectionName();
        m_db->close();
        delete m_db;
        m_db = nullptr;
        QSqlDatabase::removeDatabase(connName);
    }
}

QSqlDatabase *MySQLData::getDatabase() const { return m_db; }

MySQLData::ConnParams MySQLData::currentParams() const
{
    if (m_db)
        return {m_db->userName(), m_db->password(), m_db->hostName(), m_db->databaseName()};
    return m_params;
}

bool MySQLData::testConnection()
{
    if (!m_db) return false;
    if (m_db->isOpen()) m_db->close();
    return m_db->open();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  IMPORT asynchrone
 *
 *  Lance importThread() dans le thread pool. L'UI reste fluide.
 *  L'accès à la BD se fait via des connexions SQL créées dans le thread.
 * ══════════════════════════════════════════════════════════════════════════ */

void MySQLData::importDataAsync()
{
    const ConnParams params = currentParams();

    auto *watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher]() {
        const QString err = watcher->result();
        watcher->deleteLater();
        emit importFinished(err.isEmpty(), err.isEmpty()
                                               ? tr("Import réussi")
                                               : err);
    });

    watcher->setFuture(QtConcurrent::run([params]() -> QString {
        return MySQLData::importThread(params);
    }));
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Logique d'import dans le thread (static — pas d'accès à 'this')
 *
 *  Connexions créées ici, dans ce thread, puis fermées/supprimées à la fin.
 *  Utilise std::unique_ptr pour DatabaseManager → pas de fuite mémoire.
 * ────────────────────────────────────────────────────────────────────────── */
QString MySQLData::importThread(const ConnParams &p)
{
    /* ── Connexion MySQL dans ce thread ─────────────────────────────────── */
    const QString mysqlConn = threadConnName("import_mysql");
    {
        QSqlDatabase mysql = QSqlDatabase::addDatabase("QMYSQL", mysqlConn);
        mysql.setUserName(p.username);
        mysql.setPassword(p.password);
        mysql.setHostName(p.hostname);
        mysql.setDatabaseName(p.dbName);

        if (!mysql.open()) {
            const QString err = mysql.lastError().text();
            mysql.close();
            QSqlDatabase::removeDatabase(mysqlConn);
            return tr("Connexion MySQL impossible : ") + err;
        }

        /* ── Connexion SQLite locale dans ce thread ──────────────────────── */
        const QString sqliteConn = threadConnName("import_sqlite");
        auto dbSaver = std::make_unique<DatabaseManager>();
        QSqlError initErr = dbSaver->initDb(sqliteConn);
        if (initErr.type() != QSqlError::NoError) {
            mysql.close();
            QSqlDatabase::removeDatabase(mysqlConn);
            return tr("Impossible d'ouvrir la base locale : ") + initErr.text();
        }

        /* ── Import participants ─────────────────────────────────────────── */
        {
            QSqlQuery q(mysql);
            q.prepare("SELECT email, password, firstname, lastname, year, gender "
                      "FROM participants");
            if (!q.exec()) {
                mysql.close();
                QSqlDatabase::removeDatabase(mysqlConn);
                return tr("Erreur requête participants : ") + q.lastError().text();
            }
            while (q.next()) {
                const QString email     = q.value(0).toString();
                const QString password  = q.value(1).toString();
                const QString firstname = q.value(2).toString();
                const QString lastname  = q.value(3).toString();
                const QString year      = q.value(4).toString();
                const int     genreId   = [&]() {
                    const QString g = q.value(5).toString();
                    if (g == "M") return 1;
                    if (g == "F") return 2;
                    return 3;
                }();

                if (!dbSaver->isParticipantExist(email))
                    dbSaver->addParticipant(lastname, firstname, email, password, year, genreId);
            }
        }

        /* ── Import courses ──────────────────────────────────────────────── */
        {
            QSqlQuery q(mysql);
            q.prepare("SELECT * FROM races");
            if (!q.exec()) {
                mysql.close();
                QSqlDatabase::removeDatabase(mysqlConn);
                return tr("Erreur requête courses : ") + q.lastError().text();
            }
            while (q.next()) {
                const int     raceId     = q.value(0).toInt();
                const int     deptId     = q.value(1).toInt();
                const QString name       = q.value(2).toString();
                const QDateTime date     = QDateTime::fromString(q.value(3).toString(), Qt::ISODate);
                const QString location   = q.value(4).toString();
                const QString lon        = q.value(5).toString();
                const QString lat        = q.value(6).toString();
                const int     diff       = q.value(7).toInt();
                const int     type       = q.value(8).toInt();
                const int     book       = q.value(9).toInt();

                if (!dbSaver->isRaceExist(name))
                    dbSaver->addRace(raceId, deptId, name, date, location,
                                     lon, lat, diff, type, book);
            }
        }

        /* ── Import inscriptions (participant ↔ course) ──────────────────── */
        {
            QSqlQuery q(mysql);
            q.prepare("SELECT p_race.participant_id, p_race.race_id "
                      "FROM participants AS p "
                      "JOIN participants_races AS p_race ON p_race.participant_id = p.id");
            if (q.exec()) {
                while (q.next()) {
                    const int pid = q.value(0).toInt();
                    const int rid = q.value(1).toInt();
                    if (!dbSaver->isParticipantRaceExist(pid))
                        dbSaver->addParticipantRace(pid, rid);
                }
            }
        }

        mysql.close();
        QSqlDatabase::removeDatabase(mysqlConn);
    } /* dbSaver détruit ici (unique_ptr) — connexion fermée proprement */

    return {}; /* succès */
}

/* ══════════════════════════════════════════════════════════════════════════
 *  EXPORT asynchrone
 * ══════════════════════════════════════════════════════════════════════════ */

void MySQLData::exportDataAsync()
{
    const ConnParams params  = currentParams();
    const int        raceId  = RaceManager::getInstance()->getRaceId();

    auto *watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher]() {
        const QString err = watcher->result();
        watcher->deleteLater();
        emit exportFinished(err.isEmpty(), err.isEmpty()
                                               ? tr("Export réussi")
                                               : err);
    });

    watcher->setFuture(QtConcurrent::run([params, raceId]() -> QString {
        return MySQLData::exportThread(params, raceId);
    }));
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Logique d'export dans le thread
 * ────────────────────────────────────────────────────────────────────────── */
QString MySQLData::exportThread(const ConnParams &p, int raceId)
{
    const QString sqliteConn = threadConnName("export_sqlite");
    auto dbLocal = std::make_unique<DatabaseManager>();
    QSqlError initErr = dbLocal->initDb(sqliteConn);
    if (initErr.type() != QSqlError::NoError)
        return tr("Impossible d'ouvrir la base locale : ") + initErr.text();

    const QString mysqlConn = threadConnName("export_mysql");
    {
        QSqlDatabase mysql = QSqlDatabase::addDatabase("QMYSQL", mysqlConn);
        mysql.setUserName(p.username);
        mysql.setPassword(p.password);
        mysql.setHostName(p.hostname);
        mysql.setDatabaseName(p.dbName);

        if (!mysql.open()) {
            const QString err = mysql.lastError().text();
            mysql.close();
            QSqlDatabase::removeDatabase(mysqlConn);
            return tr("Connexion MySQL impossible : ") + err;
        }

        /* Lire les résultats locaux et les pousser vers MySQL */
        QSqlQuery queryLocal(dbLocal->getDb());
        queryLocal.prepare("SELECT * FROM participant_races_data WHERE race_id=?");
        queryLocal.addBindValue(raceId);

        if (!queryLocal.exec()) {
            mysql.close();
            QSqlDatabase::removeDatabase(mysqlConn);
            return tr("Erreur lecture résultats locaux : ") + queryLocal.lastError().text();
        }

        while (queryLocal.next()) {
            QSqlQuery ins(mysql);
            ins.prepare("INSERT INTO leaderboard_races "
                        "(race_id, participant_id, beacons, start_time, end_time, points) "
                        "VALUES(?, ?, ?, ?, ?, ?)");
            ins.addBindValue(raceId);
            ins.addBindValue(queryLocal.value(1));
            ins.addBindValue(queryLocal.value(2));
            ins.addBindValue(queryLocal.value(4));
            ins.addBindValue(queryLocal.value(5));
            ins.addBindValue(queryLocal.value(3));

            if (!ins.exec()) {
                const QString err = ins.lastError().text();
                mysql.close();
                QSqlDatabase::removeDatabase(mysqlConn);
                return tr("Erreur insertion résultat : ") + err;
            }
        }

        mysql.close();
        QSqlDatabase::removeDatabase(mysqlConn);
    }

    return {};
}
