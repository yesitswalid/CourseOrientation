# 🏗️ Architecture CourseOrientation

Documentation technique complète de l'architecture du projet.

## 📑 Table des matières

- [Vue d'ensemble](#vue-densemble)
- [Couches d'architecture](#couches-darchitecture)
- [Composants principaux](#composants-principaux)
- [Flux de données](#flux-de-données)
- [Patterns de conception](#patterns-de-conception)
- [Modèle de données](#modèle-de-données)
- [Communication réseau](#communication-réseau)
- [Points d'amélioration](#points-damélioration)

---

## Vue d'ensemble

CourseOrientation suit une **architecture en couches** avec séparation des responsabilités :

```
┌─────────────────────────────────────────┐
│  Couche Présentation (UI)               │
│  ├─ Fenêtres & Formulaires              │
│  └─ Interactions utilisateur             │
├─────────────────────────────────────────┤
│  Couche Métier (Business Logic)         │
│  ├─ RaceManager (Singleton)             │
│  └─ Gestion état application             │
├─────────────────────────────────────────┤
│  Couche Accès Données (Data Access)     │
│  ├─ DatabaseManager (SQLite)            │
│  ├─ MySQLData                           │
│  └─ SQLiteConverter                     │
├─────────────────────────────────────────┤
│  Couche Réseau (Network)                │
│  ├─ Server (UDP)                        │
│  └─ Communication portiques              │
└─────────────────────────────────────────┘
```

---

## Couches d'architecture

### 1. Couche Présentation

**Responsabilité** : Afficher l'interface et capturer les entrées utilisateur

**Composants** :
- `Application` (QMainWindow) - Fenêtre principale
- `InscriptionForm` - Formulaire inscription
- `GestionParticipant` - Gestion CRUD participants
- `GestionPortique` - Gestion portiques RFID
- `GestionCourse` - Gestion courses
- `ConfigForm` - Configuration base de données

**Caractéristiques** :
- ✅ Utilise Qt Signals/Slots
- ✅ Communication via events
- ✅ Pas de logique métier

### 2. Couche Métier

**Responsabilité** : Orchestrer la logique applicative

**Composants** :
- `RaceManager` (Singleton) - Gestion course active
- `Configuration` - Paramètres globaux

**Pattern Singleton** :
```cpp
class RaceManager : public QObject {
    static RaceManager* instance;
public:
    static RaceManager* getInstance() {
        if (!instance) instance = new RaceManager();
        return instance;
    }
};
```

### 3. Couche Accès Données

**Responsabilité** : Gérer la persistance et la synchronisation

**Composants** :
- `DatabaseManager` - CRUD SQLite
- `MySQLData` - Export/Import MySQL
- `SQLiteConverter` - Conversion JSON

**Fonctionnalités** :
- ✅ Prepared statements (prévention SQL injection)
- ✅ Transactions
- ✅ Validation de données

### 4. Couche Réseau

**Responsabilité** : Communication avec portiques et serveur

**Composants** :
- `Server` (Singleton UDP) - Portiques RFID
- MySQLData - Synchronisation serveur

**Protocoles** :
- UDP (Portiques) - Port 1234
- TCP (MySQL) - Port 3306

---

## Composants principaux

### Application (Fenêtre principale)

```cpp
class Application : public QMainWindow {
    // Managers et services
    GestionParticipant *gestion_participant;
    InscriptionForm *inscription_form;
    GestionPortique *gestion_portique;
    ConfigForm *config_form;
    
    // Accès données
    DatabaseManager *m_db;
    MySQLData *m_mydb;
    SQLiteConverter *m_sqlite;
    
    // Métier
    RaceManager *race;
    Configuration *configuration;
    
    // Slots
    void on_actionInscription_triggered();
    void on_actionGestion_des_participants_triggered();
    void on_actionExporter_triggered();
    void on_actionImporter_triggered();
};
```

**Cycle de vie** :
1. Constructeur → Initialisation UI
2. `init()` → Création services
3. `initRaces()` → Chargement courses
4. Destructeur → Nettoyage mémoire

### RaceManager (Singleton)

Gère la **course sélectionnée** de l'application :

```cpp
class RaceManager : public QObject {
    static RaceManager *instance;
    
    struct Race {
        int raceId;
        int departmentId;
        QString name;
        QString date;
        QString location;
        // ...
    };
    
public:
    static RaceManager* getInstance();
    
    void setRaceSelected(bool selected);
    bool isRaceSelected() const;
    
    int getRaceId() const;
    QString getRaceName() const;
    
    void setRace(int id, int deptId, QString name, ...);
    QVector<Race>& getRaces();
};
```

**Pattern Singleton** :
- ✅ Garantit une seule instance
- ✅ Accès global
- ✅ Initialisation lazy

### DatabaseManager

Abstraction SQLite pour toutes les opérations BD :

```cpp
class DatabaseManager {
    QSqlDatabase m_db;
    
public:
    QSqlError initDb();
    
    // Participants
    void addParticipant(const QString &lastname, ...);
    bool isParticipantExist(const QString &mail);
    void removeParticipant(const int id);
    
    // Courses
    void addRace(int race_id, QString name, ...);
    bool isRaceExist(const QString &raceName);
    
    // Checkpoints
    void addParticipantCheckpoint(...);
    bool hasParticipantCheckpoint(...);
    
    // Timing
    void setDepartTimeParticipant(int id, QDateTime time);
    void setFinishTimeParticipant(int id, QDateTime time);
};
```

**Sécurité** :
- ✅ Prepared statements partout
- ✅ Validation email unique
- ✅ Contraintes clés étrangères

### Server (UDP Singleton)

Communication avec portiques RFID :

```cpp
class Server : public QObject {
    QUdpSocket *socket;
    QHostAddress lecteurSender;
    int lecteurPort;
    
public:
    static Server* getInstance();
    void init();
    void send(QByteArray data);
    
signals:
    void getCardId(QString);
    void getResultDataParticipant(QString, int, int);
    
private slots:
    void readPendingDiagrams();
};
```

**Communication** :
- Port : 1234
- Protocole : UDP
- Format : `[BADGE_ID|TIMESTAMP|STATUS]`

---

## Flux de données

### Flux d'inscription

```
User Input (Formulaire)
    ↓
InscriptionForm::validerInscription()
    ↓
DatabaseManager::isParticipantExist()
    ├─ OUI → Erreur "Email existe"
    └─ NON → Continuer
    ↓
DatabaseManager::addParticipant()
    ↓
DatabaseManager::addParticipantRace()
    ↓
SQLite BD (local)
    ↓
Confirmation utilisateur
```

### Flux portique RFID

```
Portique → Badge RFID détecté
    ↓
Server::readPendingDiagrams() (UDP Port 1234)
    ↓
Parse message [ID|TIME|STATUS]
    ↓
Emit signal getCardId()
    ↓
GestionPortique::getCardId() (Slot)
    ↓
DatabaseManager::setDepartTimeParticipant()
    ↓
SQLite BD
    ↓
Envoyer ACK au portique
```

### Flux export MySQL

```
User: "Exporter"
    ↓
Application::on_actionExporter_triggered()
    ↓
MySQLData::exportData()
    ├─ Connexion MySQL
    ├─ Lire SQLite local
    ├─ Formatter SQL
    ├─ Exécuter INSERT
    └─ Retour statut
    ↓
Message confirmation/erreur
```

---

## Patterns de conception

### 1. Singleton (RaceManager, Server)

**Utilisation** :
```cpp
RaceManager::getInstance()->setRace(...);
Server::getInstance()->init();
```

**Avantages** :
- ✅ Instance unique garantie
- ✅ Accès global
- ✅ Initialisation lazy

### 2. MVC (Application)

**Modèle** : RaceManager (état)  
**Vue** : application.ui (formulaires)  
**Contrôleur** : Application (logique)

### 3. Observer (Qt Signals/Slots)

```cpp
// Server émet signal
connect(Server::getInstance(), 
        SIGNAL(getCardId(QString)),
        GestionPortique,
        SLOT(processCardId(QString)));
```

### 4. Factory (DatabaseManager)

Création et gestion des tables SQLite dans `initDb()`.

---

## Modèle de données

### Diagramme ER simplifié

```
┌──────────────────┐
│   PARTICIPANTS   │
├──────────────────┤
│ id (PK)          │
│ lastname         │
│ firstname        │
│ mail (UNIQUE)    │
│ password         │
│ year             │
│ genre_id (FK)    │
└──────────────────┘
         ↑
         │
         1-N
         │
    ┌────┴─────────────────┐
    │                      │
    ▼                      ▼
┌──────────────────┐  ┌──────────────────────┐
│ PARTICIPANT_     │  │ PARTICIPANT_RACES_   │
│ RACES            │  │ DATA                 │
├──────────────────┤  ├──────────────────────┤
│ id (PK)          │  │ race_id (PK,FK)      │
│ participant_id   │  │ participant_id (PK)  │
│ race_id          │  │ beacons              │
│ finger           │  │ points               │
│ bid              │  │ start                │
└──────────────────┘  │ end                  │
                      └──────────────────────┘
         ▲                      ▲
         │                      │
         N-1                    N-1
         │                      │
         └────────┬─────────────┘
                  │
         ┌────────▼──────────┐
         │     RACES          │
         ├────────────────────┤
         │ id (PK)            │
         │ id_department      │
         │ name (UNIQUE)      │
         │ date               │
         │ location           │
         │ gps_longitude      │
         │ gps_latitude       │
         │ difficulty         │
         │ type               │
         │ book               │
         └────────────────────┘
```

### Relations

- **Participants → Courses** (N-N via participant_races)
- **Participants → Données Course** (1-N via participant_races_data)
- **Participants → Checkpoints** (1-N via checkpoints)

---

## Communication réseau

### UDP (Portiques RFID)

**Endpoint** : localhost:1234

**Message Format** :
```
[ID_BADGE|TIMESTAMP|STATUS]
```

**Exemple** :
```
[001ABC5F|2024-01-15T10:30:45.123|DEPART]
[001ABC5F|2024-01-15T11:45:30.456|ARRIVEE]
```

**Réponse** :
```
ACK|OK|Participant recorded
```

### MySQL (Serveur web)

**Config** :
```
Host: 192.168.1.100
Port: 3306
User: admin
Database: coursorient
```

**Operations** :
- `importData()` - Récupère courses depuis MySQL
- `exportData()` - Envoie résultats à MySQL

---

## Points d'amélioration

### 🔴 Priorité HAUTE

1. **Includes .cpp dans .h** (application.h)
   - Cause : Définitions multiples (ODR violation)
   - Fix : Inclure .h à la place

2. **Credentials en dur** (application.cpp:75)
   - Cause : Sécurité faible
   - Fix : Variables environnement ou config.json chiffré

3. **Single-threaded database**
   - Cause : UI peut freeze sur opérations longues
   - Fix : Utiliser QThread workers

### 🟡 Priorité MOYENNE

4. **Configuration.h vide** - À remplir
5. **MySQLConverter.h vide** - À supprimer ou implémenter
6. **Pas de const-correctness** - À améliorer
7. **Pas de gestion d'erreurs réseau** - À ajouter
8. **Pas de logging** - À implémenter
9. **Pas de tests unitaires** - À créer

### 🟢 Priorité BASSE

10. **Code duplication** - Refactoriser patterns répétitifs
11. **Documentation Doxygen** - Ajouter comments
12. **Refactoring gestionparticipant.cpp** (27KB) - Trop volumineux

---

## Dépendances entre modules

```
Application (Main)
├── UI Modules
│   ├── InscriptionForm ↓ DatabaseManager
│   ├── GestionParticipant ↓ DatabaseManager
│   ├── GestionPortique ↓ Server
│   └── ConfigForm ↓ MySQLData
│
├── Data Layer
│   ├── DatabaseManager ↓ RaceManager
│   ├── MySQLData ↓ DatabaseManager
│   └── SQLiteConverter ↓ DatabaseManager
│
├── Business Layer
│   └── RaceManager (Singleton)
│
└── Network Layer
    └── Server (Singleton UDP)
```

**Règles** :
- ❌ UI ne parle pas à BD directement
- ❌ BD ne fait pas opérations réseau
- ✅ Tous les modules accèdent RaceManager
- ✅ Server émet signals (découplage)

---

## Conclusion

CourseOrientation a une **architecture solide** mais nécessite des améliorations pour la **production** :

| Aspect | État | Action |
|--------|------|--------|
| Architecture | ✅ Bonne | Maintenir |
| Sécurité | ⚠️ Faible | Corriger includes, credentials |
| Performance | ⚠️ OK | Ajouter threading |
| Testabilité | ❌ Faible | Créer tests |
| Documentation | ⚠️ Partielle | Compléter |

**Score global** : 6.5/10 (Bon pour projet scolaire, À améliorer pour production)
