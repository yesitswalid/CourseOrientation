# CourseOrientation

[![Build & Release](https://github.com/yesitswalid/CourseOrientation/actions/workflows/release.yml/badge.svg)](https://github.com/yesitswalid/CourseOrientation/actions/workflows/release.yml)
[![Qt](https://img.shields.io/badge/Qt-6.x-41CD52?logo=qt&logoColor=white)](https://www.qt.io)
[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)](https://en.cppreference.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-informational)](https://github.com/yesitswalid/CourseOrientation/releases)
[![License](https://img.shields.io/github/license/yesitswalid/CourseOrientation)](LICENSE)

Application de gestion de **courses d'orientation** : inscription des participants, suivi en temps réel via RFID/UDP, synchronisation avec un serveur MySQL distant et export des résultats.

---

## Fonctionnalités

| Catégorie | Détail |
|---|---|
| **Participants** | Inscription, mise à jour, suppression, attribution de dossard |
| **Courses** | Import depuis MySQL, sélection de la course active |
| **Portique RFID** | Réception badge NFC via UDP (lecteur externe) |
| **Port série** | Lecture dossard via RS-232 / USB-Serial |
| **Synchronisation** | Import/export **asynchrone** vers MySQL (UI non bloquée) |
| **Simulation** | Mode test avec données JSON intégrées |
| **Sécurité** | Mot de passe obfusqué dans la config, hachage SHA-256 |
| **Thème sombre** | Interface moderne via QSS (Catppuccin Mocha) |
| **Logs** | Logger fichier thread-safe (niveaux Debug → Critical) |

---

## Architecture

```
src/
├── ui/           Fenêtres Qt (application, participants, portique…)
├── business/     RaceManager singleton + modèles de données
├── data/         DatabaseManager (SQLite), MySQLData (async), SQLiteConverter
├── network/      Serveur UDP + lecture port série portique
├── config/       AppConfig (env vars + JSON), Configuration JSON
├── core/         Constantes globales
└── utilities/    Logger, Workers Qt (threading), CredentialsManager
tests/            42 tests unitaires (Qt Test)
styles/           Thème QSS
```

---

## Téléchargement

Dernière version sur la page [**Releases**](https://github.com/yesitswalid/CourseOrientation/releases) :

| Plateforme | Fichier | Installation |
|---|---|---|
| Windows 10/11 x64 | `CourseOrientation-vX.Y.Z-windows-x64.zip` | Extraire → lancer `CourseOrientation.exe` |
| Linux x86_64 | `CourseOrientation-vX.Y.Z-linux-x86_64.AppImage` | `chmod +x *.AppImage && ./CourseOrientation-*.AppImage` |

---

## Compilation depuis les sources

### Prérequis communs

- Qt 6.5+ avec les modules : `sql`, `network`, `serialport`, `concurrent`, `widgets`
- MSVC 2022 (Windows) ou GCC 10+ (Linux)
- MySQL client (optionnel) pour le driver `QMYSQL`

### Windows

```bat
:: Ouvrir "x64 Native Tools Command Prompt for VS 2022"
git clone https://github.com/yesitswalid/CourseOrientation.git
cd CourseOrientation
qmake CourseOrientation.pro CONFIG+=release
nmake release
:: Packager (windeployqt requis dans PATH)
scripts\build-windows.bat
```

### Linux

```bash
sudo apt-get install -y libgl1-mesa-dev libudev-dev libxkbcommon-dev libmysqlclient-dev

git clone https://github.com/yesitswalid/CourseOrientation.git
cd CourseOrientation
qmake CourseOrientation.pro CONFIG+=release
make -j$(nproc)
# Packager en AppImage
bash scripts/build-linux.sh
```

---

## Tests unitaires

```bash
cd tests
qmake tests.pro && make
./tst_courseorientation -v2
```

| Suite | Tests | Couverture |
|---|---|---|
| TestConfiguration | 8 | Lecture/écriture JSON, persistance |
| TestRaceManager | 10 | Machine à états DEFAULT/RAZ/DATA |
| TestCredentialsManager | 10 | Hachage SHA-256, vérification |
| TestAppConfig | 6 | Chargement JSON, variables d'environnement |
| TestDatabase | 8 | CRUD participants/courses (SQLite `:memory:`) |

---

## Installation du driver MySQL

L'application fonctionne **sans MySQL** pour la gestion locale (création de courses,
inscriptions, suivi RFID). MySQL n'est nécessaire que pour synchroniser avec un
serveur distant via les fonctions **Import / Export**.

Au démarrage, si le driver `QMYSQL` est absent, ces fonctions sont **désactivées
automatiquement** et un message d'information s'affiche.

### Windows

Le driver est inclus dans les releases pré-compilées. En cas d'installation manuelle :

1. Télécharger `qsqlmysql.dll` + `libmysql.dll` pour Qt 6.7.3 / MSVC2019 :
   [thecodemonkey86/qt_mysql_driver](https://github.com/thecodemonkey86/qt_mysql_driver/releases)
2. Placer `qsqlmysql.dll` dans `<dossier_app>/sqldrivers/`
3. Placer `libmysql.dll` à côté de `CourseOrientation.exe`

### Linux

```bash
# Ubuntu / Debian
sudo apt-get install -y libqt6sql6-mysql libmysqlclient21

# Fedora / RHEL
sudo dnf install qt6-qtbase-mysql mariadb-connector-c
```

---

## Configuration

### Connexion MySQL

Menu **Paramètre → Configuration BDD** :

| Champ | Valeur par défaut |
|---|---|
| IP | `127.0.0.1` |
| Port | `3306` |
| Utilisateur | `root` |
| Mot de passe | obfusqué (XOR + Base64) dans `Configuration/config.json` |

### Variables d'environnement (override)

```bash
export DB_HOST=192.168.1.100
export DB_PORT=3306
export DB_USER=admin
export DB_PASSWORD=secret
export DB_NAME=coursorient
export LOG_LEVEL=debug
```

### Port série (portique physique)

Par défaut : `/dev/ttyUSB0` (Linux) · `COM3` (Windows).
Personnalisable :

```
qmake DEFINES+=SERIAL_PORT_DEFAULT=\\\"COM5\\\"
```

---

## Release automatique (GitHub Actions)

```bash
git tag v1.0.0
git push origin v1.0.0
```

GitHub Actions compile Windows + Linux et crée la Release avec les artefacts.

---

## Licence

Voir [LICENSE](LICENSE).
