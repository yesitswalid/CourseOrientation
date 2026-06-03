QT += core gui sql widgets network serialport concurrent

requires(qtConfig(tableview))

CONFIG += c++17

win32: DEFINES += NOMINMAX WIN32_LEAN_AND_MEAN

# ── Chemins d'inclusion ───────────────────────────────────────────────────────
# Chaque sous-répertoire de src/ est ajouté pour que les #include "xxx.h"
# fonctionnent sans chemin complet depuis n'importe quel fichier source.
INCLUDEPATH += \
    src \
    src/ui \
    src/business \
    src/data \
    src/network \
    src/config \
    src/core \
    src/utilities/logging \
    src/utilities/threading \
    src/utilities/security

# ── Sources ───────────────────────────────────────────────────────────────────
SOURCES += \
    main.cpp \
    src/ui/application.cpp \
    src/ui/configform.cpp \
    src/ui/gestionparticipant.cpp \
    src/ui/gestionportique.cpp \
    src/ui/inscriptionform.cpp \
    src/business/racemanager.cpp \
    src/data/databasemanager.cpp \
    src/data/SQLiteConverter.cpp \
    src/data/mysqldata.cpp \
    src/network/server.cpp \
    src/network/portique.cpp \
    src/config/configuration.cpp \
    src/config/appconfig.cpp \
    src/utilities/logging/filelogger.cpp \
    src/utilities/security/credentialsmanager.cpp \
    src/utilities/threading/dbworker.cpp \
    src/utilities/threading/qthreadworker.cpp

# ── En-têtes ──────────────────────────────────────────────────────────────────
HEADERS += \
    src/ui/application.h \
    src/ui/configform.h \
    src/ui/gestionparticipant.h \
    src/ui/gestionportique.h \
    src/ui/inscriptionform.h \
    src/business/racemanager.h \
    src/data/databasemanager.h \
    src/data/SQLiteConverter.h \
    src/data/mysqldata.h \
    src/network/server.h \
    src/network/portique.h \
    src/config/configuration.h \
    src/config/appconfig.h \
    src/core/constants.h \
    src/business/models/participant.h \
    src/business/models/race.h \
    src/data/database/idatabaseaccess.h \
    src/network/server/inetworkserver.h \
    src/utilities/logging/ilogger.h \
    src/utilities/logging/filelogger.h \
    src/utilities/security/credentialsmanager.h \
    src/utilities/threading/qthreadworker.h \
    src/utilities/threading/dbworker.h

# ── Formulaires .ui ───────────────────────────────────────────────────────────
FORMS += \
    src/ui/application.ui \
    src/ui/configform.ui \
    src/ui/gestionparticipant.ui \
    src/ui/gestionportique.ui \
    src/ui/inscriptionform.ui

# ── Ressources ────────────────────────────────────────────────────────────────
RESOURCES += resources.qrc

# ── Port série par défaut selon la plateforme ─────────────────────────────────
win32:  DEFINES += SERIAL_PORT_DEFAULT=\\\"COM3\\\"
unix:   DEFINES += SERIAL_PORT_DEFAULT=\\\"/dev/ttyUSB0\\\"

# ── Déploiement ───────────────────────────────────────────────────────────────
qnx:              target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
