/* Fichier inclus par tst_main.cpp — ne pas compiler séparément */

#include <QtTest>
#include "racemanager.h"

/**
 * @class TestRaceManager
 * @brief Tests unitaires de la machine à états et des données de course.
 */
class TestRaceManager : public QObject
{
    Q_OBJECT

    RaceManager *rm = nullptr;

private slots:

    void init()
    {
        rm = RaceManager::getInstance();
        rm->setMode(RaceManager::DEFAULT);
        rm->setRaceSelected(false);
        rm->setRaceId(-1);
    }

    void testModeInitialDefault()
    {
        QCOMPARE(rm->getMode(), RaceManager::DEFAULT);
    }

    void testTransitionDefaultVersRaz()
    {
        rm->setMode(RaceManager::RAZ);
        QCOMPARE(rm->getMode(), RaceManager::RAZ);
    }

    void testTransitionRazVersData()
    {
        rm->setMode(RaceManager::RAZ);
        rm->setMode(RaceManager::DATA);
        QCOMPARE(rm->getMode(), RaceManager::DATA);
    }

    void testAucuneCourseInitiale()
    {
        QVERIFY(!rm->isRaceSelected());
    }

    void testSelectionCourse()
    {
        rm->setRaceSelected(true);
        QVERIFY(rm->isRaceSelected());
        rm->setRaceSelected(false);
        QVERIFY(!rm->isRaceSelected());
    }

    void testAffectationCourse()
    {
        rm->setRace(42, 75, "Trail du Mont Blanc", "2024-06-15T09:00:00",
                    "Chamonix", 6.8650f, 45.9237f, 3, 1, 200);

        QCOMPARE(rm->getRaceId(),       42);
        QCOMPARE(rm->getDepartmentId(), 75);
        QCOMPARE(rm->getName(),         QString("Trail du Mont Blanc"));
        QCOMPARE(rm->getLocation(),     QString("Chamonix"));
        QCOMPARE(rm->getDifficulty(),   3);
        QCOMPARE(rm->getBook(),         200);
    }

    void testCoordGPS()
    {
        rm->setGPSLongitude(2.3488f);
        rm->setGPSLatitude(48.8534f);
        QVERIFY(qAbs(rm->getGPSLongitude() - 2.3488f)  < 0.001f);
        QVERIFY(qAbs(rm->getGPSLatitude()  - 48.8534f) < 0.001f);
    }

    void testListeCourses()
    {
        QVector<RaceManager::Race> liste;
        RaceManager::Race r1; r1.raceId = 1; r1.name = "A";
        RaceManager::Race r2; r2.raceId = 2; r2.name = "B";
        liste << r1 << r2;
        rm->setRaces(liste);
        QCOMPARE(rm->getRaces().size(), 2);
        QCOMPARE(rm->getRaces().at(0).name, QString("A"));
    }

    void testSingleton()
    {
        QVERIFY(RaceManager::getInstance() == rm);
    }

    void testConstantesDistinctes()
    {
        QVERIFY(RaceManager::DEFAULT != RaceManager::RAZ);
        QVERIFY(RaceManager::RAZ     != RaceManager::DATA);
    }
};
