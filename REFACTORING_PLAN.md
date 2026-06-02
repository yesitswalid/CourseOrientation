# 🔄 Plan de Refactorisation CourseOrientation

## 📋 Critiques à adresser

| # | Critique | Sévérité | Impact | Solution |
|----|----------|----------|--------|----------|
| 1 | Includes .cpp dans .h | 🔴 HAUTE | ODR violation, compilation lente | Extraire définitions en .h propres |
| 2 | Credentials en dur | 🔴 HAUTE | Sécurité compromise | Variables d'env + config chiffré |
| 3 | Single-threaded BD/Réseau | 🔴 HAUTE | UI freeze possible | QThread workers |
| 4 | Pas de gestion d'erreurs réseau | 🟡 MOYENNE | Crash UDP | Try/catch + reconnection |
| 5 | Configuration.h vide | 🟡 MOYENNE | Confusion | Implémenter properly |
| 6 | Pas de logging | 🟡 MOYENNE | Debug difficile | Logger singleton |
| 7 | Pas de const-correctness | 🟡 MOYENNE | Optimisations perdues | Audit + fix |
| 8 | Pas de tests unitaires | 🟡 MOYENNE | Régression risk | Framework Qt Test |
| 9 | Code duplication | 🟠 BASSE | Maintenance | Patterns helper |
| 10 | Fichiers volumineux | 🟠 BASSE | Compréhension | Split en modules |

---

## 🏗️ Nouvelle Architecture (Modulaire)

```
CourseOrientation/
│
├── 📁 src/
│   ├── 📁 core/              # Noyau applicatif
│   │   ├── application.h/cpp
│   │   ├── constants.h
│   │   └── globals.h
│   │
│   ├── 📁 ui/                # Interface utilisateur
│   │   ├── 📁 windows/
│   │   │   ├── mainwindow.h/cpp
│   │   │   ├── inscriptionwindow.h/cpp
│   │   │   └── settingswindow.h/cpp
│   │   ├── 📁 dialogs/
│   │   │   └── configdialog.h/cpp
│   │   ├── 📁 widgets/
│   │   │   ├── participanttable.h/cpp
│   │   │   └── racecombobox.h/cpp
│   │   └── resources.qrc
│   │
│   ├── 📁 business/          # Logique métier
│   │   ├── 📁 managers/
│   │   │   ├── racemanager.h/cpp (Singleton)
│   │   │   ├── participantmanager.h/cpp
│   │   │   └── eventmanager.h/cpp
│   │   ├── 📁 models/
│   │   │   ├── participant.h
│   │   │   ├── race.h
│   │   │   ├── checkpoint.h
│   │   │   └── datamodels.h
│   │   └── 📁 validators/
│   │       ├── emailvalidator.h/cpp
│   │       └── datavalidator.h/cpp
│   │
│   ├── 📁 data/              # Accès données
│   │   ├── 📁 database/
│   │   │   ├── idatabaseaccess.h (Interface)
│   │   │   ├── sqlitedatabase.h/cpp
│   │   │   ├── mysqldatabase.h/cpp
│   │   │   ├── dbconstants.h
│   │   │   └── migrations/
│   │   │       └── v1_init_schema.sql
│   │   ├── 📁 converters/
│   │   │   ├── jsondataconverter.h/cpp
│   │   │   └── csvdataconverter.h/cpp
│   │   └── 📁 repositories/
│   │       ├── participantrepository.h/cpp
│   │       ├── racerepository.h/cpp
│   │       └── checkpointrepository.h/cpp
│   │
│   ├── 📁 network/           # Communication réseau
│   │   ├── 📁 server/
│   │   │   ├── inetworkserver.h (Interface)
│   │   │   ├── udpserver.h/cpp (Singleton)
│   │   │   └── connectionpool.h/cpp
│   │   ├── 📁 sync/
│   │   │   ├── datasynchronizer.h/cpp
│   │   │   └── syncworker.h/cpp
│   │   └── networkconstants.h
│   │
│   ├── 📁 config/            # Configuration & paramètres
│   │   ├── appconfig.h/cpp
│   │   ├── databaseconfig.h/cpp
│   │   ├── networkconfig.h/cpp
│   │   └── config.json (template)
│   │
│   ├── 📁 utilities/         # Utilitaires
│   │   ├── 📁 logging/
│   │   │   ├── ilogger.h (Interface)
│   │   │   ├── filelogger.h/cpp
│   │   │   └── consolelogger.h/cpp
│   │   ├── 📁 threading/
│   │   │   ├── qthreadworker.h/cpp (Base class)
│   │   │   ├── dbworker.h/cpp
│   │   │   └── networkworker.h/cpp
│   │   ├── 📁 security/
│   │   │   ├── credentialsmanager.h/cpp
│   │   │   └── cryptoutils.h/cpp
│   │   ├── datetime.h/cpp
│   │   ├── stringutils.h/cpp
│   │   └── fileutils.h/cpp
│   │
│   └── main.cpp
│
├── 📁 tests/                 # Tests unitaires
│   ├── test_database.cpp
│   ├── test_managers.cpp
│   ├── test_validators.cpp
│   ├── test_config.cpp
│   └── CMakeLists.txt
│
├── 📁 resources/             # Ressources (icônes, etc)
│   └── assets/
│
├── 📄 CMakeLists.txt         # Build Qt/CMake
├── 📄 CourseOrientation.pro  # QMake (legacy)
├── 📄 README.md
├── 📄 ARCHITECTURE.md
├── 📄 REFACTORING.md         # Ce document
└── 📄 LICENSE

```

---

## 🔧 Implémentation détaillée

### Phase 1: Séparation des concerns (2-3 semaines)

#### 1.1 Interfaces abstraites

**data/database/idatabaseaccess.h**
```cpp
#ifndef IDATABASEACCESS_H
#define IDATABASEACCESS_H

#include <QString>
#include <QList>
#include <QSqlError>
#include "models/participant.h"
#include "models/race.h"

class IDatabaseAccess {
public:
    virtual ~IDatabaseAccess() = default;
    
    // Lifecycle
    virtual QSqlError initialize() = 0;
    virtual void close() = 0;
    virtual bool isConnected() const = 0;
    
    // Participants
    virtual bool addParticipant(const Participant &p) = 0;
    virtual bool updateParticipant(const Participant &p) = 0;
    virtual bool deleteParticipant(int id) = 0;
    virtual Participant getParticipant(int id) const = 0;
    virtual QList<Participant> getAllParticipants() const = 0;
    virtual bool participantExists(const QString &email) const = 0;
    
    // Races
    virtual bool addRace(const Race &r) = 0;
    virtual bool updateRace(const Race &r) = 0;
    virtual QList<Race> getAllRaces() const = 0;
    virtual Race getRace(int id) const = 0;
};

#endif
```

**network/server/inetworkserver.h**
```cpp
#ifndef INETWORKSERVER_H
#define INETWORKSERVER_H

#include <QObject>
#include <QString>

class INetworkServer : public QObject {
    Q_OBJECT
public:
    virtual ~INetworkServer() = default;
    
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
signals:
    void cardIdReceived(const QString &cardId);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);
};

#endif
```

#### 1.2 Models propres

**business/models/participant.h**
```cpp
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QString>
#include <QDate>

struct Participant {
    int id = -1;
    QString lastName;
    QString firstName;
    QString email;
    QString password;  // Hashed
    QString year;
    int genderId = -1;
    QDateTime registrationDate;
    bool isActive = true;
    
    // Validation
    bool isValid() const;
    QString getFullName() const;
};

#endif
```

#### 1.3 Configuration centralisée

**config/appconfig.h**
```cpp
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QJsonObject>

class AppConfig {
public:
    static AppConfig& instance();
    
    bool loadFromFile(const QString &filePath);
    bool loadFromEnvironment();
    
    // Database config
    QString dbHost() const;
    int dbPort() const;
    QString dbUser() const;
    QString dbPassword() const;
    QString dbName() const;
    QString dbLocalPath() const;
    
    // Network config
    int networkPort() const;
    QString networkHost() const;
    int networkTimeout() const;
    
    // Logging config
    QString logLevel() const;
    QString logFilePath() const;
    
private:
    AppConfig() = default;
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;
    
    QJsonObject m_config;
    
    QString getEnv(const QString &key, const QString &defaultValue = "") const;
};

#endif
```

**config/appconfig.cpp**
```cpp
#include "appconfig.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <cstdlib>

AppConfig& AppConfig::instance() {
    static AppConfig instance;
    return instance;
}

bool AppConfig::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    m_config = doc.object();
    file.close();
    
    return true;
}

bool AppConfig::loadFromEnvironment() {
    // Priorité: Environment > fichier config
    // Les env vars surpassent le fichier
    return true;
}

QString AppConfig::dbHost() const {
    return getEnv("DB_HOST", m_config["database"].toObject()["host"].toString());
}

QString AppConfig::getEnv(const QString &key, const QString &defaultValue) const {
    const char *value = std::getenv(key.toStdString().c_str());
    return value ? QString::fromStdString(value) : defaultValue;
}
```

### Phase 2: Threading (2 semaines)

#### 2.1 Worker pattern

**utilities/threading/qthreadworker.h**
```cpp
#ifndef QTHREADWORKER_H
#define QTHREADWORKER_H

#include <QObject>
#include <QThread>
#include <QString>

class QThreadWorker : public QObject {
    Q_OBJECT
public:
    QThreadWorker(QObject *parent = nullptr);
    virtual ~QThreadWorker();
    
    void start();
    void stop();
    bool isRunning() const;
    
protected:
    virtual void doWork() = 0;
    void logDebug(const QString &msg);
    void logError(const QString &msg);
    
signals:
    void finished();
    void errorOccurred(const QString &error);
    void workStarted();
    
private slots:
    void onThreadStarted();
    void onThreadFinished();
    
private:
    QThread *m_thread = nullptr;
    bool m_running = false;
};

#endif
```

**utilities/threading/dbworker.h**
```cpp
#ifndef DBWORKER_H
#define DBWORKER_H

#include "qthreadworker.h"
#include "data/database/idatabaseaccess.h"
#include "business/models/participant.h"
#include <QList>

class DatabaseWorker : public QThreadWorker {
    Q_OBJECT
public:
    explicit DatabaseWorker(IDatabaseAccess *db, QObject *parent = nullptr);
    
    void queryAllParticipants();
    void addParticipant(const Participant &p);
    void updateParticipant(const Participant &p);
    
signals:
    void participantsLoaded(const QList<Participant> &participants);
    void participantAdded(const Participant &p);
    void queryFailed(const QString &error);
    
protected:
    void doWork() override;
    
private:
    IDatabaseAccess *m_db = nullptr;
    enum QueryType { LoadParticipants, AddParticipant, UpdateParticipant };
    QueryType m_currentQuery;
    Participant m_pendingParticipant;
};

#endif
```

#### 2.2 Managers refactorisés

**business/managers/racemanager.h**
```cpp
#ifndef RACEMANAGER_H
#define RACEMANAGER_H

#include <QObject>
#include "business/models/race.h"
#include "data/repositories/racerepository.h"
#include <memory>

class RaceManager : public QObject {
    Q_OBJECT
    
    using RaceRepositoryPtr = std::unique_ptr<RaceRepository>;
    
public:
    static RaceManager& instance();
    
    // Thread-safe operations
    Q_INVOKABLE bool selectRace(int raceId);
    Q_INVOKABLE int getSelectedRaceId() const;
    Q_INVOKABLE Race getSelectedRace() const;
    Q_INVOKABLE bool isRaceSelected() const;
    
    // Async loading
    void loadRaces();
    
signals:
    void raceSelected(const Race &race);
    void racesLoaded(const QList<Race> &races);
    void errorOccurred(const QString &error);
    
private:
    RaceManager(QObject *parent = nullptr);
    ~RaceManager() = default;
    
    RaceManager(const RaceManager&) = delete;
    RaceManager& operator=(const RaceManager&) = delete;
    
    int m_selectedRaceId = -1;
    RaceRepositoryPtr m_repository;
    mutable QMutex m_mutex;
};

#endif
```

### Phase 3: Logging & Security (1 semaine)

#### 3.1 Logger singleton

**utilities/logging/ilogger.h**
```cpp
#ifndef ILOGGER_H
#define ILOGGER_H

#include <QString>

enum class LogLevel { Debug, Info, Warning, Error, Critical };

class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void log(LogLevel level, const QString &message) = 0;
    virtual void debug(const QString &msg) = 0;
    virtual void info(const QString &msg) = 0;
    virtual void warning(const QString &msg) = 0;
    virtual void error(const QString &msg) = 0;
    virtual void critical(const QString &msg) = 0;
};

#endif
```

**utilities/logging/filelogger.h/cpp**
```cpp
#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "ilogger.h"
#include <QFile>
#include <QMutex>
#include <memory>

class FileLogger : public ILogger {
public:
    static FileLogger& instance();
    
    bool initialize(const QString &filePath);
    
    void log(LogLevel level, const QString &message) override;
    void debug(const QString &msg) override { log(LogLevel::Debug, msg); }
    void info(const QString &msg) override { log(LogLevel::Info, msg); }
    void warning(const QString &msg) override { log(LogLevel::Warning, msg); }
    void error(const QString &msg) override { log(LogLevel::Error, msg); }
    void critical(const QString &msg) override { log(LogLevel::Critical, msg); }
    
private:
    FileLogger() = default;
    ~FileLogger() = default;
    
    QString formatMessage(LogLevel level, const QString &message) const;
    QString levelToString(LogLevel level) const;
    
    std::unique_ptr<QFile> m_file;
    mutable QMutex m_mutex;
};

#endif
```

#### 3.2 Credentials manager

**utilities/security/credentialsmanager.h**
```cpp
#ifndef CREDENTIALSMANAGER_H
#define CREDENTIALSMANAGER_H

#include <QString>

class CredentialsManager {
public:
    static CredentialsManager& instance();
    
    // Load from environment or encrypted config
    bool loadCredentials();
    
    QString getDatabasePassword() const;
    QString getApiKey() const;
    
    // Hash password for DB storage
    static QString hashPassword(const QString &password);
    static bool verifyPassword(const QString &password, const QString &hash);
    
private:
    CredentialsManager() = default;
    CredentialsManager(const CredentialsManager&) = delete;
    CredentialsManager& operator=(const CredentialsManager&) = delete;
    
    QString m_dbPassword;
    QString m_apiKey;
};

#endif
```

### Phase 4: Tests & Validation (1 semaine)

**tests/test_database.cpp**
```cpp
#include <QtTest>
#include "data/database/sqlitedatabase.h"
#include "business/models/participant.h"

class TestDatabase : public QObject {
    Q_OBJECT
    
private slots:
    void initTestCase();
    void testAddParticipant();
    void testUpdateParticipant();
    void testDeleteParticipant();
    void testParticipantExists();
    void cleanupTestCase();
    
private:
    std::unique_ptr<SQLiteDatabase> m_db;
};

void TestDatabase::initTestCase() {
    m_db = std::make_unique<SQLiteDatabase>(":memory:");
    QVERIFY(m_db->initialize().type() == QSqlError::NoError);
}

void TestDatabase::testAddParticipant() {
    Participant p;
    p.lastName = "Dupont";
    p.firstName = "Jean";
    p.email = "jean@test.com";
    
    QVERIFY(m_db->addParticipant(p));
    QVERIFY(m_db->participantExists("jean@test.com"));
}

void TestDatabase::cleanupTestCase() {
    m_db.reset();
}

#include "test_database.moc"
```

---

## 📊 Avant/Après comparaison

| Aspect | Avant | Après |
|--------|-------|-------|
| **Includes** | .cpp en .h ❌ | Séparation nette ✅ |
| **Threading** | Single-threaded ❌ | Workers + QThread ✅ |
| **Security** | Hardcoded ❌ | Env vars + encryption ✅ |
| **Logging** | Pas de logs ❌ | Logger singleton ✅ |
| **Error handling** | Minimal ❌ | Try/catch complet ✅ |
| **Testabilité** | Faible ❌ | Interfaces + tests ✅ |
| **Modulabilité** | Couplée ❌ | Découplée (interfaces) ✅ |
| **Réutilisabilité** | Limitée ❌ | Componentes séparés ✅ |
| **Performance** | UI freeze ❌ | UI fluide ✅ |
| **Documentation** | Manquante ❌ | Complète ✅ |

---

## 🚀 Roadmap d'implémentation

### Week 1: Préparation
- [ ] Créer structure de dossiers
- [ ] Valider CMakeLists.txt
- [ ] Setup tests framework

### Week 2-3: Phase 1 (Interfaces)
- [ ] Interfaces abstraites
- [ ] Models de données
- [ ] Config centralisée

### Week 4: Phase 2 (Threading)
- [ ] QThreadWorker base
- [ ] DatabaseWorker
- [ ] NetworkWorker

### Week 5: Phase 3 (Logging + Security)
- [ ] Logger singleton
- [ ] CredentialsManager
- [ ] Audit const-correctness

### Week 6: Phase 4 (Tests)
- [ ] Tests unitaires
- [ ] Integration tests
- [ ] Performance tests

### Week 7: Migration
- [ ] Remplacer ancien code
- [ ] Validation complète
- [ ] Deploy en production

---

## ✅ Checklist finale

- [ ] **Pas d'includes .cpp dans .h**
- [ ] **Credentials en variables d'env**
- [ ] **Threading BD + réseau**
- [ ] **Logging système complet**
- [ ] **100% const-correct**
- [ ] **Interfaces abstraites**
- [ ] **Tests unitaires (>80% coverage)**
- [ ] **Documentation Doxygen**
- [ ] **Performance : 0 UI freeze**
- [ ] **Architecture modulable**
