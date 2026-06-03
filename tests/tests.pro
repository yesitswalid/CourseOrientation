# ============================================================
# Projet de tests unitaires — CourseOrientation
# ============================================================
# Compilation et exécution :
#   cd tests
#   qmake tests.pro && make
#   ./tst_courseorientation -v2
# ============================================================

QT       += testlib sql widgets
QT       -= gui
CONFIG   += c++17 console
CONFIG   -= app_bundle
TEMPLATE  = app
TARGET    = tst_courseorientation

win32: DEFINES += NOMINMAX

# Les fichiers de test sont inclus dans tst_main.cpp (une seule TU → un seul .moc)
SOURCES  += tst_main.cpp

# Sources du projet principal requis par les tests
SOURCES  += \
    ../src/config/configuration.cpp         \
    ../src/business/racemanager.cpp         \
    ../src/data/databasemanager.cpp         \
    ../src/config/appconfig.cpp             \
    ../src/utilities/security/credentialsmanager.cpp

HEADERS  += \
    ../src/config/configuration.h           \
    ../src/business/racemanager.h           \
    ../src/data/databasemanager.h           \
    ../src/config/appconfig.h               \
    ../src/core/constants.h                 \
    ../src/utilities/security/credentialsmanager.h

# Chemins d'inclusion — même logique que le projet principal
INCLUDEPATH += \
    ..                                      \
    ../src                                  \
    ../src/business                         \
    ../src/data                             \
    ../src/config                           \
    ../src/core                             \
    ../src/utilities/security

DEFINES  += SRCDIR=\\\"$$PWD/\\\"
