/* Fichier inclus par tst_main.cpp — ne pas compiler séparément */

#include <QtTest>
#include <QFile>
#include "configuration.h"

/**
 * @class TestConfiguration
 * @brief Tests unitaires de la classe Configuration (persistance JSON).
 */
class TestConfiguration : public QObject
{
    Q_OBJECT

    static const QString FICHIER_TEST;

private slots:

    void cleanup()
    {
        QFile::remove("Configuration/" + FICHIER_TEST);
    }

    void testSetGetChaineDeCaracteres()
    {
        Configuration config(FICHIER_TEST);
        config.set("prenom", QString("Alice"));
        QCOMPARE(config.get("prenom").toString(), QString("Alice"));
    }

    void testSetGetEntier()
    {
        Configuration config(FICHIER_TEST);
        config.set("port", 3306);
        QCOMPARE(config.get("port").toInt(), 3306);
    }

    void testPersistanceFichier()
    {
        { Configuration c1(FICHIER_TEST); c1.set("persistance", QString("ok")); }
        Configuration c2(FICHIER_TEST);
        QCOMPARE(c2.get("persistance").toString(), QString("ok"));
    }

    void testPlusieursValeurs()
    {
        Configuration config(FICHIER_TEST);
        config.set("ip",   QString("192.168.1.1"));
        config.set("port", 5432);
        QCOMPARE(config.get("ip").toString(),  QString("192.168.1.1"));
        QCOMPARE(config.get("port").toInt(),   5432);
    }

    void testCleInexistante()
    {
        Configuration config(FICHIER_TEST);
        QJsonValue val = config.get("cle_absente");
        QVERIFY(val.isUndefined() || val.isNull());
    }

    void testEcrasementValeur()
    {
        Configuration config(FICHIER_TEST);
        config.set("compteur", 1);
        config.set("compteur", 99);
        QCOMPARE(config.get("compteur").toInt(), 99);
    }

    void testSetAll()
    {
        Configuration config(FICHIER_TEST);
        config.set("ancienne", QString("valeur"));
        QJsonObject obj;
        obj["nouvelle"] = "val";
        config.setAll(obj);
        QVERIFY(config.get("ancienne").isUndefined() || config.get("ancienne").isNull());
        QCOMPARE(config.get("nouvelle").toString(), QString("val"));
    }

    void testGetAllNonVide()
    {
        Configuration config(FICHIER_TEST);
        config.set("a", QString("1"));
        QVERIFY(!config.getAll().isEmpty());
    }
};

const QString TestConfiguration::FICHIER_TEST = "tst_temp_unit.json";
