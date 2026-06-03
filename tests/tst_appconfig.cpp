/* Fichier inclus par tst_main.cpp — ne pas compiler séparément */

#include <QtTest>
#include <QFile>
#include "appconfig.h"

/**
 * @class TestAppConfig
 * @brief Tests de chargement de configuration depuis JSON et variables d'environnement.
 */
class TestAppConfig : public QObject
{
    Q_OBJECT

    static const QString FICHIER;

    void creerFichier(const QString &json)
    {
        QFile f(FICHIER);
        f.open(QIODevice::WriteOnly | QIODevice::Text);
        f.write(json.toUtf8());
        f.close();
    }

private slots:

    void cleanup() { QFile::remove(FICHIER); }

    void testChargementFichierValide()
    {
        creerFichier(R"({
            "database": {"host":"10.0.0.1","port":5432,"user":"usr","password":"pw","name":"db"},
            "network":  {"port":9999,"host":"0.0.0.0","timeout":8000},
            "logging":  {"level":"debug","filePath":"test.log"}
        })");

        AppConfig &cfg = AppConfig::instance();
        QVERIFY(cfg.loadFromFile(FICHIER));
        QCOMPARE(cfg.dbHost(),      QString("10.0.0.1"));
        QCOMPARE(cfg.dbPort(),      5432);
        QCOMPARE(cfg.dbUser(),      QString("usr"));
        QCOMPARE(cfg.dbName(),      QString("db"));
        QCOMPARE(cfg.networkPort(), 9999);
        QCOMPARE(cfg.logLevel(),    QString("debug"));
        QCOMPARE(cfg.logFilePath(), QString("test.log"));
    }

    void testFichierInexistant()
    {
        AppConfig &cfg = AppConfig::instance();
        QVERIFY(!cfg.loadFromFile("/chemin/impossible/fichier.json"));
    }

    void testTimeoutPositif()
    {
        QVERIFY(AppConfig::instance().networkTimeout() > 0);
    }

    void testCheminLocalNonVide()
    {
        QVERIFY(!AppConfig::instance().dbLocalPath().isEmpty());
    }

    void testLoadFromEnvironmentRetourneVrai()
    {
        QVERIFY(AppConfig::instance().loadFromEnvironment());
    }

    void testFichierPartiel()
    {
        creerFichier(R"({"database": {"host": "172.16.0.1"}})");
        AppConfig &cfg = AppConfig::instance();
        cfg.loadFromFile(FICHIER);
        QCOMPARE(cfg.dbHost(), QString("172.16.0.1"));
        QVERIFY(cfg.dbPort() > 0);
    }
};

const QString TestAppConfig::FICHIER = "tst_appconfig_temp.json";
