/**
 * @file tst_main.cpp
 * @brief Suite de tests unitaires CourseOrientation.
 *
 * Toutes les classes de test sont incluses ici dans une seule unité de
 * traduction afin que MOC puisse générer un seul fichier .moc couvrant
 * l'ensemble. Chaque bloc #include apporte la définition d'une classe de test.
 *
 * Compilation :
 * @code
 *   cd tests && qmake tests.pro && make
 *   ./tst_courseorientation -v2
 * @endcode
 */

#include <QCoreApplication>
#include <QtTest>
#include <QDateTime>
#include <QDir>

/* ────────────────────────────────────────────────────────────────────────────
 * Inclusion des fichiers de test dans cet ordre
 * ──────────────────────────────────────────────────────────────────────────── */
#include "tst_configuration.cpp"
#include "tst_racemanager.cpp"
#include "tst_credentials.cpp"
#include "tst_appconfig.cpp"
#include "tst_database.cpp"

/* MOC unique pour toutes les classes Q_OBJECT définies ci-dessus */
#include "tst_main.moc"

/* ────────────────────────────────────────────────────────────────────────────
 * Point d'entrée
 * ──────────────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("tst_courseorientation");

    /* Créer le dossier Configuration/ dont certains tests ont besoin */
    QDir().mkpath("Configuration");

    int resultat = 0;

    qInfo() << "\n╔══════════════════════════════════════════════╗";
    qInfo() << "║   Suite de tests — CourseOrientation v1.0   ║";
    qInfo() << "╚══════════════════════════════════════════════╝";

    /* ── Configuration ───────────────────────────────────────────────────── */
    qInfo() << "\n── TestConfiguration ──────────────────────────";
    { TestConfiguration t; resultat |= QTest::qExec(&t, argc, argv); }

    /* ── RaceManager ─────────────────────────────────────────────────────── */
    qInfo() << "\n── TestRaceManager ────────────────────────────";
    { TestRaceManager t; resultat |= QTest::qExec(&t, argc, argv); }

    /* ── CredentialsManager ──────────────────────────────────────────────── */
    qInfo() << "\n── TestCredentialsManager ─────────────────────";
    { TestCredentialsManager t; resultat |= QTest::qExec(&t, argc, argv); }

    /* ── AppConfig ───────────────────────────────────────────────────────── */
    qInfo() << "\n── TestAppConfig ──────────────────────────────";
    { TestAppConfig t; resultat |= QTest::qExec(&t, argc, argv); }

    /* ── DatabaseManager ─────────────────────────────────────────────────── */
    qInfo() << "\n── TestDatabase ───────────────────────────────";
    { TestDatabase t; resultat |= QTest::qExec(&t, argc, argv); }

    /* ── Résumé ──────────────────────────────────────────────────────────── */
    qInfo() << "\n══════════════════════════════════════════════";
    if (resultat == 0)
        qInfo() << "✓  Tous les tests ont réussi.";
    else
        qCritical() << "✗  Échec — code de sortie :" << resultat;
    qInfo() << "══════════════════════════════════════════════\n";

    return resultat;
}
