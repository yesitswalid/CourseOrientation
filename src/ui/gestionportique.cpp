#include "ui_gestionportique.h"
#include "gestionportique.h"
#include "racemanager.h"
#include "server.h"
#include "configuration.h"
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

GestionPortique::GestionPortique(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GestionPortique)
{
    ui->setupUi(this);
    ui->modeLabel->setText("Mode : " + getModeToString());
    setupSerialPortSelector();
}

GestionPortique::~GestionPortique()
{
    delete ui;
}

QString GestionPortique::getModeToString() const
{
    return (RaceManager::getInstance()->getMode() == RaceManager::DATA) ? "DATA" : "RAZ";
}

QString GestionPortique::getSelectedSerialPort() const
{
    return m_portCombo ? m_portCombo->currentData().toString() : QString();
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Sélecteur de port série — créé programmatiquement et inséré au-dessus
 *  des boutons RAZ/DATA existants.
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::setupSerialPortSelector()
{
    /* ── Création des widgets ────────────────────────────────────────────── */
    auto *groupBox = new QGroupBox("Port série du portique", this);
    groupBox->setObjectName("groupPortSerie");

    auto *labelInfo = new QLabel(
        "Choisissez le port série utilisé par le portique RFID :",
        groupBox);
    labelInfo->setWordWrap(true);

    m_portCombo = new QComboBox(groupBox);
    m_portCombo->setObjectName("portCombo");
    m_portCombo->setMinimumWidth(220);

    m_refreshButton = new QPushButton("Rafraîchir", groupBox);
    m_refreshButton->setObjectName("refreshPortsButton");
    m_refreshButton->setCursor(Qt::PointingHandCursor);
    m_refreshButton->setToolTip("Re-détecter les ports série disponibles");

    m_portStatusLabel = new QLabel(groupBox);
    m_portStatusLabel->setObjectName("portStatusLabel");
    m_portStatusLabel->setStyleSheet("color: #a6adc8; font-size: 11px; font-style: italic;");

    /* ── Layout du sélecteur ─────────────────────────────────────────────── */
    auto *hLayout = new QHBoxLayout();
    hLayout->setSpacing(8);
    hLayout->addWidget(m_portCombo, 1);
    hLayout->addWidget(m_refreshButton);

    auto *vLayout = new QVBoxLayout(groupBox);
    vLayout->setContentsMargins(12, 16, 12, 12);
    vLayout->setSpacing(8);
    vLayout->addWidget(labelInfo);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_portStatusLabel);

    /* ── Insertion dans le layout principal au-dessus des boutons ────────── */
    /* Le centralWidget n'a pas toujours de layout direct (Qt Designer crée
     * souvent un sous-widget "verticalLayoutWidget" avec geometry fixe).
     * On cherche le premier QBoxLayout disponible dans la hiérarchie. */
    QBoxLayout *cibleLayout = nullptr;
    if (auto *cw = centralWidget()) {
        cibleLayout = qobject_cast<QBoxLayout *>(cw->layout());
        if (!cibleLayout) {
            /* Chercher dans les enfants directs */
            for (QObject *child : cw->children()) {
                if (auto *w = qobject_cast<QWidget *>(child)) {
                    if (auto *bl = qobject_cast<QBoxLayout *>(w->layout())) {
                        cibleLayout = bl;
                        break;
                    }
                }
            }
        }
    }

    if (cibleLayout) {
        cibleLayout->insertWidget(0, groupBox);
    } else if (centralWidget()) {
        /* Fallback ultime : remplacer le contenu par un layout vertical */
        auto *newLayout = new QVBoxLayout(centralWidget());
        newLayout->addWidget(groupBox);
    }

    /* ── Connexions ──────────────────────────────────────────────────────── */
    connect(m_refreshButton, &QPushButton::clicked,
            this, &GestionPortique::onRefreshPorts);
    connect(m_portCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GestionPortique::onPortSelectionChanged);

    /* ── Détection initiale + restauration du port précédemment choisi ──── */
    onRefreshPorts();

    Configuration config;
    const QString portSauvegarde = config.get("port_serie").toString();
    if (!portSauvegarde.isEmpty()) {
        const int idx = m_portCombo->findData(portSauvegarde);
        if (idx >= 0) {
            m_portCombo->setCurrentIndex(idx);
            m_portStatusLabel->setText("Port restauré depuis la configuration : " + portSauvegarde);
        }
    }
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Détecte les ports série disponibles et remplit la combobox.
 *  Affiche pour chaque port : nom système + description (ex: USB-Serial CH340).
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::onRefreshPorts()
{
    /* Bloquer temporairement le signal pour éviter une émission spurious */
    const bool wasBlocked = m_portCombo->blockSignals(true);
    const QString portActuel = m_portCombo->currentData().toString();

    m_portCombo->clear();
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    if (ports.isEmpty()) {
        m_portCombo->addItem("Aucun port détecté", QString());
        m_portStatusLabel->setText("⚠ Aucun port série trouvé. Branchez le portique et cliquez sur Rafraîchir.");
        m_portCombo->setEnabled(false);
    } else {
        m_portCombo->setEnabled(true);
        for (const QSerialPortInfo &info : ports) {
            QString libelle = info.portName();
            if (!info.description().isEmpty())
                libelle += "  —  " + info.description();
            else if (!info.manufacturer().isEmpty())
                libelle += "  —  " + info.manufacturer();
            m_portCombo->addItem(libelle, info.portName());
        }

        m_portStatusLabel->setText(
            QString("%1 port(s) détecté(s)").arg(ports.size()));

        /* Restaurer la sélection précédente si possible */
        const int idx = m_portCombo->findData(portActuel);
        if (idx >= 0)
            m_portCombo->setCurrentIndex(idx);
    }

    m_portCombo->blockSignals(wasBlocked);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Émet le signal serialPortChanged() et sauvegarde le choix en config.
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::onPortSelectionChanged(int index)
{
    if (index < 0) return;

    const QString port = m_portCombo->itemData(index).toString();
    if (port.isEmpty()) return;

    /* Sauvegarder le choix dans Configuration/config.json */
    Configuration config;
    config.set(QString("port_serie"), port);

    m_portStatusLabel->setText("Port sélectionné : " + port);

    emit serialPortChanged(port);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Bouton RAZ : passe le lecteur UDP en mode attribution des badges
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::on_razButton_clicked()
{
    if (!Server::getInstance()->getClient()->isValid()) {
        QMessageBox::information(this, "Portique",
            "Aucun lecteur connecté. Vérifiez la communication UDP.");
        return;
    }

    Server::getInstance()->send(QByteArray("R"));
    RaceManager::getInstance()->setMode(RaceManager::RAZ);
    ui->modeLabel->setText("Mode : " + getModeToString());
    QMessageBox::information(this, "Portique", "Le lecteur est maintenant en mode RAZ.");
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Bouton DATA : passe le lecteur UDP en mode collecte des résultats
 * ────────────────────────────────────────────────────────────────────────── */
void GestionPortique::on_dataButton_clicked()
{
    if (!Server::getInstance()->getClient()->isValid()) {
        QMessageBox::information(this, "Portique",
            "Aucun lecteur connecté. Vérifiez la communication UDP.");
        return;
    }

    Server::getInstance()->send(QByteArray("D"));
    RaceManager::getInstance()->setMode(RaceManager::DATA);
    ui->modeLabel->setText("Mode : " + getModeToString());
    QMessageBox::information(this, "Portique", "Le lecteur est maintenant en mode DATA.");
}
