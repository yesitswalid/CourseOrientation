#include "configform.h"
#include "ui_configform.h"
#include <QMessageBox>
#include <QtSql>
#include <QStyle>

#define PORT_DEFAULT 3306

/* ──────────────────────────────────────────────────────────────────────────
 *  Obfuscation du mot de passe (XOR + Base64)
 *  Ce n'est pas du chiffrement cryptographique, mais empêche la lecture
 *  directe du mot de passe dans le fichier config.json.
 *  Préfixe "ENC:" permet de détecter une valeur déjà encodée.
 * ────────────────────────────────────────────────────────────────────────── */

static const QByteArray CLE_XOR = QByteArrayLiteral("CO_App_2024_Key#!");
static const QString    PREFIXE  = "ENC:";

QString ConfigForm::encoderMotDePasse(const QString &mdp)
{
    QByteArray data = mdp.toUtf8();
    for (int i = 0; i < data.size(); ++i)
        data[i] = data[i] ^ CLE_XOR[i % CLE_XOR.size()];
    return PREFIXE + QString::fromLatin1(data.toBase64());
}

QString ConfigForm::decoderMotDePasse(const QString &encoded)
{
    if (!encoded.startsWith(PREFIXE))
        return encoded; /* Valeur déjà en clair (config existante) */

    QByteArray data = QByteArray::fromBase64(
        encoded.mid(PREFIXE.length()).toLatin1());
    for (int i = 0; i < data.size(); ++i)
        data[i] = data[i] ^ CLE_XOR[i % CLE_XOR.size()];
    return QString::fromUtf8(data);
}

bool ConfigForm::estEncode(const QString &valeur)
{
    return valeur.startsWith(PREFIXE);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Constructeur
 * ────────────────────────────────────────────────────────────────────────── */

ConfigForm::ConfigForm()
    : ui(new Ui::ConfigForm)
{
    ui->setupUi(this);
    setWindowTitle("Configuration — Base de données MySQL");

    configuration = new Configuration();

    /* ── Sécurité : champ mot de passe masqué par défaut ─────────────────── */
    ui->mdpEdit->setEchoMode(QLineEdit::Password);
    ui->mdpEdit->setPlaceholderText("Mot de passe...");

    /* ── Bouton œil pour afficher/masquer le mot de passe ────────────────── */
    m_toggleAction = ui->mdpEdit->addAction(
        style()->standardIcon(QStyle::SP_DialogHelpButton),
        QLineEdit::TrailingPosition
    );
    m_toggleAction->setToolTip("Afficher / Masquer le mot de passe");
    connect(m_toggleAction, &QAction::triggered,
            this, &ConfigForm::onToggleVisibiliteMdp);

    /* ── Charger la configuration existante ──────────────────────────────── */
    QJsonObject obj       = configuration->getAll();
    /* Qt 6 : éviter QJsonValueRef — utiliser .value().toObject() directement */
    QJsonObject mObjValues = obj.value("base_de_donnees").toObject();

    /* Valeurs par défaut pour les clés manquantes */
    QJsonObject defaults;
    defaults["ip"]          = "127.0.0.1";
    defaults["port"]        = PORT_DEFAULT;
    defaults["user"]        = "root";
    defaults["mot_de_passe"] = "";
    defaults["database"]    = "";

    for (const QString &cle : defaults.keys()) {
        if (!mObjValues.contains(cle))
            mObjValues.insert(cle, defaults.value(cle));
    }

    QJsonObject mObj;
    mObj["base_de_donnees"] = mObjValues;
    configuration->setAll(mObj);

    /* Pré-remplir les champs */
    if (mObjValues.contains("ip"))
        ui->ipEdit->setText(mObjValues["ip"].toString());
    if (mObjValues.contains("port"))
        ui->portEdit->setText(QString::number(mObjValues["port"].toInt()));
    if (mObjValues.contains("user"))
        ui->userEdit->setText(mObjValues["user"].toString());
    if (mObjValues.contains("database"))
        ui->dbEdit->setText(mObjValues["database"].toString());

    /* Décoder le mot de passe avant affichage */
    if (mObjValues.contains("mot_de_passe")) {
        const QString mdpStocke = mObjValues["mot_de_passe"].toString();
        ui->mdpEdit->setText(decoderMotDePasse(mdpStocke));
    }
}

void ConfigForm::init(MySQLData *data)
{
    m_db = data;
}

ConfigForm::~ConfigForm()
{
    delete ui;
    delete configuration;
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Bascule la visibilité du mot de passe
 * ────────────────────────────────────────────────────────────────────────── */
void ConfigForm::onToggleVisibiliteMdp()
{
    m_mdpVisible = !m_mdpVisible;
    ui->mdpEdit->setEchoMode(m_mdpVisible ? QLineEdit::Normal : QLineEdit::Password);
    m_toggleAction->setIcon(style()->standardIcon(
        m_mdpVisible ? QStyle::SP_DialogCloseButton : QStyle::SP_DialogHelpButton));
    m_toggleAction->setToolTip(m_mdpVisible
        ? "Masquer le mot de passe"
        : "Afficher le mot de passe");
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Sauvegarde de la configuration avec mot de passe obfusqué
 * ────────────────────────────────────────────────────────────────────────── */
void ConfigForm::on_continueButton_clicked()
{
    if (ui->ipEdit->text().isEmpty() || ui->portEdit->text().isEmpty() ||
        ui->userEdit->text().isEmpty() || ui->mdpEdit->text().isEmpty() ||
        ui->dbEdit->text().isEmpty())
    {
        QMessageBox::critical(this, "Base de données",
                              "Veuillez remplir tous les champs.");
        return;
    }

    QJsonObject obj        = configuration->getAll();
    QJsonObject mObjValues = obj.value("base_de_donnees").toObject();

    const QString ipSaisie   = ui->ipEdit->text().trimmed();
    const int     portSaisi  = ui->portEdit->text().trimmed().toInt();
    const QString userSaisi  = ui->userEdit->text().trimmed();
    const QString mdpSaisi   = ui->mdpEdit->text();
    const QString dbSaisie   = ui->dbEdit->text().trimmed();

    /* Éviter la réécriture si rien n'a changé (comparer avec la version décodée) */
    const QString mdpActuel = decoderMotDePasse(mObjValues["mot_de_passe"].toString());
    if (mObjValues["ip"].toString()   == ipSaisie   &&
        mObjValues["port"].toInt()    == portSaisi  &&
        mObjValues["user"].toString() == userSaisi  &&
        mdpActuel                     == mdpSaisi   &&
        mObjValues["database"].toString() == dbSaisie)
    {
        return;
    }

    /* Obfusquer le mot de passe avant de l'enregistrer */
    mObjValues.insert("ip",           ipSaisie);
    mObjValues.insert("port",         portSaisi);
    mObjValues.insert("user",         userSaisi);
    mObjValues.insert("mot_de_passe", encoderMotDePasse(mdpSaisi));
    mObjValues.insert("database",     dbSaisie);

    QJsonObject mObj;
    mObj["base_de_donnees"] = mObjValues;
    configuration->setAll(mObj);

    QMessageBox::information(this, "Base de données",
                             "Configuration sauvegardée !");
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Test de connexion avec les valeurs saisies
 * ────────────────────────────────────────────────────────────────────────── */
void ConfigForm::on_testButton_clicked()
{
    if (ui->ipEdit->text().isEmpty() || ui->portEdit->text().isEmpty() ||
        ui->userEdit->text().isEmpty() || ui->mdpEdit->text().isEmpty() ||
        ui->dbEdit->text().isEmpty())
    {
        QMessageBox::critical(this, "Base de données",
                              "Veuillez remplir tous les champs.");
        return;
    }

    m_db->getDatabase()->setUserName(ui->userEdit->text().trimmed());
    m_db->getDatabase()->setPassword(ui->mdpEdit->text());
    m_db->getDatabase()->setHostName(ui->ipEdit->text().trimmed());
    m_db->getDatabase()->setDatabaseName(ui->dbEdit->text().trimmed());
    m_db->getDatabase()->setPort(ui->portEdit->text().toInt());

    if (m_db->getDatabase()->open())
        QMessageBox::information(this, "Base de données",
                                 "Connexion réussie !");
    else
        QMessageBox::critical(this, "Base de données",
                              "Connexion échouée — vérifiez vos informations.");
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Retourne les paramètres de connexion (mot de passe décodé)
 * ────────────────────────────────────────────────────────────────────────── */
QJsonObject ConfigForm::getDatabaseData()
{
    QJsonObject obj    = configuration->getAll();
    QJsonObject result = obj.value("base_de_donnees").toObject();

    /* Décoder le mot de passe pour l'utilisation programmatique */
    if (result.contains("mot_de_passe")) {
        result["mot_de_passe"] = decoderMotDePasse(result["mot_de_passe"].toString());
    }
    return result;
}
