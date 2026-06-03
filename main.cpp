#include "application.h"   // src/ui/application.h via INCLUDEPATH
#include "server.h"         // src/network/server.h
#include "appconfig.h"      // src/config/appconfig.h
#include "filelogger.h"     // src/utilities/logging/filelogger.h
#include <QApplication>
#include <QFile>

static void appliquerTheme(QApplication &app)
{
    QFile styleFile(":/styles/app.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
        styleFile.close();
    } else {
        qWarning() << "Thème introuvable : :/styles/app.qss";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CourseOrientation");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("CourseOrientation");

    /* ── 1. Configuration ────────────────────────────────────────────────── */
    AppConfig &cfg = AppConfig::instance();
    cfg.loadFromEnvironment();

    /* ── 2. Logger fichier ───────────────────────────────────────────────── */
    FileLogger::instance().initialize(cfg.logFilePath());
    FileLogger::instance().info("=== Démarrage de CourseOrientation ===");

    /* ── 3. Thème graphique ──────────────────────────────────────────────── */
    appliquerTheme(a);

    /* ── 4. Fenêtre principale ───────────────────────────────────────────── */
    Application w;
    w.show();

    /* ── 5. Serveur UDP ──────────────────────────────────────────────────── */
    Server::getInstance()->init();

    return a.exec();
}
