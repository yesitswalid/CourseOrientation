#QT       += core gui widgets sql

#requires(qtConfig(tableview))

QT += core gui sql widgets widgets network
requires(qtConfig(tableview))

#greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets sql


CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configform.cpp \
    configuration.cpp \
    databasemanager.cpp \
    gestioncourse.cpp \
    gestionparticipant.cpp \
    gestionportique.cpp \
    inscriptionform.cpp \
    main.cpp \
    application.cpp \
    mysqldata.cpp \
    portique.cpp \
    racemanager.cpp \
    sqliteconverter.cpp

HEADERS += \
    application.h \
    configform.h \
    databasemanager.h \
    gestioncourse.h \
    gestionparticipant.h \
    gestionportique.h \
    inscriptionform.h \
    portique.h \
    racemanager.h \
    server.h

FORMS += \
    application.ui \
    configform.ui \
    gestioncourse.ui \
    gestionparticipant.ui \
    gestionportique.ui \
    inscriptionform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
