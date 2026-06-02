# CourseOrientation 🏃‍♂️

Une application **multi-plateforme** (Linux/Windows) développée avec **Qt 6** pour gérer les inscriptions et le suivi des courses d'orientation avec balises RFID.

## 📋 Table des matières

- [🎯 Vue d'ensemble](#-vue-densemble)
- [✨ Fonctionnalités](#-fonctionnalités)
- [🏗️ Architecture](#-architecture)
- [📦 Installation](#-installation)
- [💻 Utilisation](#-utilisation)
- [📁 Structure du projet](#-structure-du-projet)
- [🛠️ Technologies](#-technologies)
- [⚙️ Configuration](#-configuration)
- [🤝 Contribution](#-contribution)
- [📞 Support](#-support)

---

## 🎯 Vue d'ensemble

**CourseOrientation** est une solution complète pour les clubs de course d'orientation permettant :
- La **pré-inscription** en ligne des participants
- L'**inscription sur site** sans connexion Internet
- Le **suivi RFID** des passages aux points de contrôle
- La **gestion locale** avec synchronisation web
- L'**export de résultats** vers un serveur central

### Contexte
Remplace les anciennes perforatrices par des **lecteurs RFID** modernes. Les participants utilisent des badges RFID, passent par des portiques pour valider départ/arrivée, et les données sont synchronisées avec un serveur web pour les classements.

---

## ✨ Fonctionnalités

### 👥 Pour les Participants
- 📝 **Pré-inscription** optionnelle à une course
- 🆗 **Inscription le jour même** sans Internet
- 🏷️ **Badge RFID** pour identifier le participant
- 🏷️ **Passage au portique RFID** pour valider départ et arrivée
- 📊 **Points de contrôle** enregistrés automatiquement

### 🎯 Pour les Organisateurs
- 🎯 **Gestion des courses** (création, sélection active)
- 👥 **Gestion des participants** (CRUD complet)
- 🔗 **Communication portiques RFID** via UDP
- 💾 **Base de données SQLite locale** (offline)
- ☁️ **Export/Import MySQL** vers serveur web
- 📈 **Classements et statistiques**
- ⚙️ **Configuration flexible** de la BD

---

## 🏗️ Architecture

### Diagramme d'architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    COUCHE PRÉSENTATION (UI)                 │
│  ┌──────────────┬──────────────┬──────────────┐             │
│  │  Inscription │ Gestion      │ Gestion      │             │
│  │  Formulaire  │ Participants │ Portique     │             │
│  └──────────────┴──────────────┴──────────────┘             │
├─────────────────────────────────────────────────────────────┤
│                 COUCHE MÉTIER (Business Logic)              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  RaceManager (Singleton)                             │   │
│  │  ├─ Courses sélectionnées                            │   │
│  │  └─ État application                                │   │
│  └──────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                    COUCHE DONNÉES (Data Access)            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  DatabaseManager (SQLite)                            │   │
│  │  ├─ CRUD Participants                               │   │
│  │  ├─ CRUD Courses                                     │   │
│  │  └─ CRUD Checkpoints                                │   │
│  └──────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                      COUCHE RÉSEAU                         │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Server (Singleton UDP)    MySQLData                 │   │
│  │  ├─ Portique RFID          ├─ Export/Import        │   │
│  │  └─ Port 1234              └─ Sync MySQL            │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### Composants Principaux

#### 1. **Couche Présentation**
| Composant | Rôle |
|-----------|------|
| `Application` | Fenêtre principale, orchestration |
| `InscriptionForm` | Formulaire d'inscription participant |
| `GestionParticipant` | Tableau & CRUD participants |
| `GestionPortique` | Gestion portiques RFID |
| `ConfigForm` | Configuration base de données |

#### 2. **Couche Métier**
| Composant | Rôle |
|-----------|------|
| `RaceManager` | Singleton - Gestion course active |
| `Configuration` | Paramètres globaux |

#### 3. **Couche Données**
| Composant | Rôle |
|-----------|------|
| `DatabaseManager` | CRUD local SQLite |
| `SQLiteConverter` | Export/Import JSON |
| `MySQLData` | Communication serveur MySQL |

#### 4. **Couche Réseau**
| Composant | Rôle |
|-----------|------|
| `Server` | Singleton UDP - Portiques RFID |

---

## 📦 Installation

### 🔧 Prérequis

**Système d'exploitation:**
- Linux (Ubuntu 20.04+) ou Windows 10+

**Qt Framework:**
- Qt 6.x ou supérieur

**Plugins Qt requis:**
```
✅ QSQLite   - Base de données SQLite (inclus)
✅ QMySQL    - Base de données MySQL
✅ QNetwork  - Communication réseau
✅ QSerialPort - Communication série
```

### 📥 Installation pas à pas

#### 1️⃣ Cloner le projet
```bash
git clone https://github.com/yesitswalid/CourseOrientation.git
cd CourseOrientation
```

#### 2️⃣ Installer Qt 6

**Linux (Ubuntu):**
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-tools-dev \
  libqt6sql6 libqt6network6 libqt6serialport6
```

**Windows/macOS:**
- Télécharger depuis [qt.io](https://www.qt.io/download)
- Sélectionner Qt 6.x lors de l'installation

#### 3️⃣ Compiler le projet

**Avec Qt Creator:**
1. Ouvrir `CourseOrientation.pro`
2. Sélectionner le kit Qt 6
3. Appuyer sur `Ctrl+B` (Build)

**En ligne de commande:**
```bash
mkdir build && cd build
qmake ../CourseOrientation.pro
make -j$(nproc)
./CourseOrientation
```

#### 4️⃣ Premier lancement

- L'app crée automatiquement `course.db` dans le dossier d'exécution
- Configurer MySQL via `Configuration → Base de données`
- Importer les courses depuis le serveur web

---

## 💻 Utilisation

### Flux d'utilisation typique

```
┌─────────────────────────┐
│   Démarrage Application │
└───────────┬─────────────┘
            │
            ▼
┌─────────────────────────┐
│  Sélectionner une Course│
└───────────┬─────────────┘
            │
     ┌──────┴──────┐
     │             │
     ▼             ▼
┌──────────┐  ┌─────────────┐
│Inscri-   │  │Gestion      │
│ption     │  │Participants │
└──────────┘  └─────────────┘
     │             │
     └──────┬──────┘
            │
            ▼
┌─────────────────────────────┐
│  Gestion Portique RFID      │
│  ├─ Départ (validation)     │
│  └─ Arrivée (enregistrement)│
└───────────┬─────────────────┘
            │
     ┌──────┴──────┐
     │             │
     ▼             ▼
┌──────────┐  ┌──────────┐
│Exporter  │  │Importer  │
│MySQL     │  │MySQL     │
└──────────┘  └──────────┘
```

### Scénarios d'utilisation

#### 📝 Ajouter un participant
1. Sélectionner une course dans le menu principal
2. Cliquer sur `Fichier → Inscription`
3. Remplir le formulaire (nom, prénom, email, etc.)
4. Valider → Enregistrement immédiat

#### 🏷️ Valider un participant au portique
1. Ouvrir `Fenêtre → Gestion Portique`
2. Placer le badge RFID devant le lecteur
3. L'app reçoit automatiquement l'ID via UDP
4. Temps de passage enregistré en BD

#### ☁️ Exporter les résultats
1. Cliquer sur `Fichier → Exporter`
2. Les données SQLite sont envoyées au serveur MySQL
3. Message de confirmation

#### 📥 Importer les courses
1. Cliquer sur `Fichier → Importer`
2. Récupère les courses depuis le serveur web
3. Met à jour le combo box des courses

---

## 📁 Structure du projet

```
CourseOrientation/
│
├── 📄 README.md                      # Documentation principale
├── 📄 ARCHITECTURE.md                # Architecture technique
├── 📄 LICENSE                        # Apache 2.0
├── 📄 CourseOrientation.pro          # Configuration QMake
├── 📄 simulation.json                # Données de simulation
│
├── 🔧 NOYAU
├── main.cpp                          # Point d'entrée
│
├── 🎨 INTERFACE (UI)
├── application.h/cpp                 # Fenêtre principale
├── application.ui                    # Designer UI
├── inscriptionform.h/cpp/ui          # Formulaire inscription
├── gestionparticipant.h/cpp/ui       # Gestion participants
├── gestionportique.h/cpp/ui          # Gestion portiques
├── gestioncourse.h/cpp/ui            # Gestion courses
├── configform.h/cpp/ui               # Configuration BD
│
├── 💾 ACCÈS AUX DONNÉES
├── databasemanager.h/cpp             # Manager SQLite
├── SQLiteConverter.h/cpp             # Convertisseur SQLite/JSON
├── mysqldata.h/cpp                   # Interface MySQL
│
├── 🎯 MÉTIER
├── racemanager.h/cpp                 # Singleton course active
├── portique.h/cpp                    # Modèle portique
├── race.h/cpp                        # Modèle course
│
├── 🌐 RÉSEAU
├── server.h/cpp                      # Singleton UDP
│
├── ⚙️ CONFIGURATION
├── configuration.h/cpp               # Paramètres
├── config.json                       # Fichier config
│
└── 🎨 RESSOURCES
    └── assets/                       # Images, icônes
```

---

## 🛠️ Technologies

| Technologie | Usage | Version |
|-------------|-------|---------|
| **Qt** | Framework GUI | 6.x |
| **C++** | Langage principal | C++11 |
| **SQLite** | Base locale | 3.x |
| **MySQL** | Base serveur | 5.7+ |
| **UDP** | Communication réseau | IPv4 |
| **JSON** | Sérialisation données | Standard |
| **QMake** | Build system | Qt |

### Dépendances Qt
```qmake
QT += core gui sql widgets network serialport
```

---

## ⚙️ Configuration

### Fichier `config.json`

```json
{
  "ip": "192.168.1.100",
  "port": 3306,
  "user": "admin",
  "mot_de_passe": "password",
  "database": "coursorient",
  "port_udp": 1234
}
```

### Variables d'environnement

```bash
# Base de données locale
export DB_PATH="./course.db"

# Serveur MySQL
export DB_HOST="192.168.1.100"
export DB_PORT=3306
export DB_USER="admin"
export DB_PASS="password"
export DB_NAME="coursorient"
```

### Configuration Portique RFID

| Paramètre | Valeur |
|-----------|--------|
| **Protocole** | UDP |
| **Port** | 1234 |
| **Adresse** | 127.0.0.1 |
| **Format** | Hex (ID badge) |

---

## 📊 Modèles de données

### Tables SQLite

```sql
-- Participants
CREATE TABLE participants (
  id INTEGER PRIMARY KEY,
  lastname VARCHAR,
  firstname VARCHAR,
  mail VARCHAR UNIQUE,
  password VARCHAR,
  year VARCHAR,
  genre_id INTEGER
);

-- Courses
CREATE TABLE races (
  id INTEGER PRIMARY KEY,
  id_department INTEGER,
  name VARCHAR UNIQUE,
  date DATETIME,
  location VARCHAR,
  gps_longitude VARCHAR,
  gps_latitude VARCHAR,
  difficulty INTEGER,
  type INTEGER,
  book INTEGER
);

-- Relation Participant ↔ Course
CREATE TABLE participant_races (
  id INTEGER PRIMARY KEY,
  participant_id INTEGER,
  race_id INTEGER,
  finger VARCHAR DEFAULT NULL,
  bid VARCHAR DEFAULT NULL,
  FOREIGN KEY(participant_id) REFERENCES participants(id),
  FOREIGN KEY(race_id) REFERENCES races(id)
);

-- Données de la course du participant
CREATE TABLE participant_races_data (
  race_id INTEGER,
  participant_id INTEGER,
  beacons INTEGER DEFAULT 0,
  points INTEGER DEFAULT 0,
  start DATETIME DEFAULT 0,
  end DATETIME DEFAULT 0,
  PRIMARY KEY(race_id, participant_id)
);

-- Points de passage (checkpoints)
CREATE TABLE checkpoints (
  id INTEGER PRIMARY KEY,
  altitude VARCHAR,
  longitude VARCHAR,
  attitude VARCHAR,
  race_id INTEGER,
  participant_id INTEGER,
  order_id INTEGER,
  points INTEGER,
  FOREIGN KEY(race_id) REFERENCES races(id),
  FOREIGN KEY(participant_id) REFERENCES participants(id)
);

-- Genres
CREATE TABLE genders (
  id INTEGER PRIMARY KEY,
  sexe VARCHAR UNIQUE
);
```

---

## 🔌 API Réseau

### Communication Portique RFID

**Format requête (Portique → Application):**
```
[ID_BADGE|TIMESTAMP|STATUS]
Exemple: [001ABC5F|2024-01-15T10:30:45|DEPART]
```

**Format réponse (Application → Portique):**
```
[ACK|STATUS|MESSAGE]
Exemple: [ACK|OK|Participant enregistré]
```

### Communication MySQL

- **Protocol**: TCP/IP
- **Port**: 3306 (par défaut)
- **Authentification**: Username/Password
- **Format**: SQL INSERT/UPDATE/SELECT

---

## 🤝 Contribution

Les contributions sont bienvenues ! Pour contribuer :

### Processus

1. **Fork** le projet
2. **Créer une branche** (`git checkout -b feature/AmazingFeature`)
3. **Commiter** les changements (`git commit -m 'Add some AmazingFeature'`)
4. **Push** vers la branche (`git push origin feature/AmazingFeature`)
5. **Ouvrir une Pull Request**

### Guidelines

- ✅ Respecter le style C++11
- ✅ Ajouter des tests unitaires
- ✅ Mettre à jour la documentation
- ✅ Commenter le code complexe

---

## 📞 Support

| Canal | Lien |
|-------|------|
| 📧 **Email** | [yesitswalid@gmail.com](mailto:yesitswalid@gmail.com) |
| 🐛 **Issues** | [GitHub Issues](https://github.com/yesitswalid/CourseOrientation/issues) |
| 💬 **Discussions** | [GitHub Discussions](https://github.com/yesitswalid/CourseOrientation/discussions) |

---

## 📝 License

Ce projet est distribué sous la licence **Apache 2.0** - voir [LICENSE](LICENSE) pour les détails.

---

**Créé avec ❤️ pour les passionnés de course d'orientation**

🏃‍♂️ **Keep running, keep tracking!**
