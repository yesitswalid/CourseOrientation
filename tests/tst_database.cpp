/* Fichier inclus par tst_main.cpp — ne pas compiler séparément */

#include <QtTest>
#include "databasemanager.h"
#include "racemanager.h"

/**
 * @class TestDatabase
 * @brief Tests CRUD sur DatabaseManager avec une base SQLite en mémoire.
 */
class TestDatabase : public QObject
{
    Q_OBJECT

    DatabaseManager m_db;
    static int      compteur;

    QString emailUnique() { return QString("tst_%1@unit.test").arg(++compteur); }

private slots:

    /* Initialisation unique — base :memory: + course de test */
    void initTestCase()
    {
        QSqlError err = m_db.initDbInMemory();
        QVERIFY2(err.type() == QSqlError::NoError,
                 qPrintable("Échec init BD : " + err.text()));

        RaceManager::getInstance()->setRaceId(1);
        RaceManager::getInstance()->setRaceSelected(true);

        /* Insérer une course de test (id=1) */
        m_db.addRace(1, 74, "Course Test",
                     QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate),
                     "Annecy", "6.1190", "45.8990", 2, 1, 50);
    }

    /* ── Participants ───────────────────────────────────────────────────── */

    void testAjouterEtTrouverParticipant()
    {
        const QString email = emailUnique();
        QVERIFY(!m_db.isParticipantExist(email));
        m_db.addParticipant("Dupont", "Jean", email, "hash", "1990-01-01", 1);
        QVERIFY(m_db.isParticipantExist(email));
    }

    void testSupprimerParticipant()
    {
        const QString email = emailUnique();
        m_db.addParticipant("Martin", "Sophie", email, "hash", "1995-05-12", 2);
        QVERIFY(m_db.isParticipantExist(email));

        QSqlQuery q = m_db.getDb().exec();
        q.prepare("SELECT id FROM participants WHERE mail=?");
        q.addBindValue(email);
        QVERIFY(q.exec() && q.next());
        m_db.removeParticipant(q.value(0).toInt());
        QVERIFY(!m_db.isParticipantExist(email));
    }

    void testParticipantInexistant()
    {
        QVERIFY(!m_db.isParticipantExist("fantome@inexistant.com"));
    }

    /* ── Inscription à une course ───────────────────────────────────────── */

    void testAjouterParticipantACourse()
    {
        const QString email = emailUnique();
        m_db.addParticipant("Leroy", "Marc", email, "hash", "1988-08-20", 1);

        QSqlQuery q = m_db.getDb().exec();
        q.prepare("SELECT id FROM participants WHERE mail=?");
        q.addBindValue(email);
        QVERIFY(q.exec() && q.next());
        const int pid = q.value(0).toInt();

        QVERIFY(!m_db.isParticipantRaceExist(pid));
        m_db.addParticipantRace(pid, 1);
        QVERIFY(m_db.isParticipantRaceExist(pid));
    }

    /* ── Courses ─────────────────────────────────────────────────────────── */

    void testExistenceCourse()
    {
        QVERIFY(m_db.isRaceExist("Course Test"));
        QVERIFY(!m_db.isRaceExist("Course Inexistante XYZ"));
    }

    /* ── Genres préchargés ──────────────────────────────────────────────── */

    void testGenresPrecharges()
    {
        QSqlQuery q = m_db.getDb().exec();
        q.prepare("SELECT COUNT(*) FROM genders");
        QVERIFY(q.exec() && q.next());
        QVERIFY(q.value(0).toInt() >= 3);
    }

    /* ── Checkpoints ─────────────────────────────────────────────────────── */

    void testCheckpointInexistant()
    {
        QVERIFY(!m_db.hasParticipantCheckpoint(999, 1, 999));
    }

    void testAjouterCheckpoint()
    {
        const QString email = emailUnique();
        m_db.addParticipant("Bernard", "Lucie", email, "hash", "2000-03-15", 2);

        QSqlQuery q = m_db.getDb().exec();
        q.prepare("SELECT id FROM participants WHERE mail=?");
        q.addBindValue(email);
        QVERIFY(q.exec() && q.next());
        const int pid = q.value(0).toInt();
        m_db.addParticipantRace(pid, 1);

        const int ordreId = 77;
        QVERIFY(!m_db.hasParticipantCheckpoint(ordreId, 1, pid));
        m_db.addParticipantCheckpoint("250", "6.865", "45.923", 1, pid, ordreId, 10);
        QVERIFY(m_db.hasParticipantCheckpoint(ordreId, 1, pid));
    }
};

int TestDatabase::compteur = 0;
