/* Fichier inclus par tst_main.cpp — ne pas compiler séparément */

#include <QtTest>
#include "credentialsmanager.h"

/**
 * @class TestCredentialsManager
 * @brief Tests du hachage SHA-256 et de la vérification de mot de passe.
 */
class TestCredentialsManager : public QObject
{
    Q_OBJECT

private slots:

    void testHachageNonVide()
    {
        QVERIFY(!CredentialsManager::hashPassword("motdepasse").isEmpty());
    }

    void testLongueurSHA256()
    {
        /* SHA-256 = 32 octets = 64 caractères hexadécimaux */
        QCOMPARE(CredentialsManager::hashPassword("test").length(), 64);
    }

    void testVerificationCorrecte()
    {
        const QString mdp  = "mon_secret_42!";
        const QString hash = CredentialsManager::hashPassword(mdp);
        QVERIFY(CredentialsManager::verifyPassword(mdp, hash));
    }

    void testVerificationIncorrecte()
    {
        const QString hash = CredentialsManager::hashPassword("correct");
        QVERIFY(!CredentialsManager::verifyPassword("incorrect", hash));
    }

    void testUniciteDesHachages()
    {
        QVERIFY(CredentialsManager::hashPassword("a") !=
                CredentialsManager::hashPassword("b"));
    }

    void testDeterminisme()
    {
        const QString mdp = "determinisme_2024";
        QCOMPARE(CredentialsManager::hashPassword(mdp),
                 CredentialsManager::hashPassword(mdp));
    }

    void testMotDePasseVide()
    {
        const QString hash = CredentialsManager::hashPassword("");
        QCOMPARE(hash.length(), 64);
        QVERIFY(CredentialsManager::verifyPassword("", hash));
        QVERIFY(!CredentialsManager::verifyPassword("x", hash));
    }

    void testCaracteresSpeciaux()
    {
        const QString mdp  = "P@$$w0rd!#&*()_+-=[];',./`~";
        const QString hash = CredentialsManager::hashPassword(mdp);
        QCOMPARE(hash.length(), 64);
        QVERIFY(CredentialsManager::verifyPassword(mdp, hash));
    }

    void testSensibleCasse()
    {
        QVERIFY(CredentialsManager::hashPassword("Password") !=
                CredentialsManager::hashPassword("password"));
    }

    void testSingleton()
    {
        QVERIFY(&CredentialsManager::instance() == &CredentialsManager::instance());
    }
};
