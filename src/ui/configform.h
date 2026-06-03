#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QWidget>
#include <QAction>
#include <QSqlDatabase>
#include <QJsonObject>
#include "configuration.h"
#include "mysqldata.h"

namespace Ui { class ConfigForm; }

/**
 * @class ConfigForm
 * @brief Formulaire de configuration de la connexion à la base MySQL distante.
 *
 * Les champs IP, port, utilisateur, mot de passe et nom de base sont
 * sauvegardés dans Configuration/config.json.
 * Le mot de passe est obfusqué (XOR + Base64) avant stockage — il n'est
 * jamais enregistré en clair.
 */
class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm();
    ~ConfigForm();

    /** @brief Retourne les paramètres de connexion depuis la configuration. */
    QJsonObject getDatabaseData();

    /** @brief Injecte l'instance MySQLData pour le test de connexion. */
    void init(MySQLData *data);

private slots:
    void on_continueButton_clicked();
    void on_testButton_clicked();
    void onToggleVisibiliteMdp();

private:
    /** @brief Obfusque le mot de passe (XOR + Base64) avant stockage. */
    static QString encoderMotDePasse(const QString &mdp);

    /** @brief Décode un mot de passe préalablement obfusqué. */
    static QString decoderMotDePasse(const QString &encoded);

    /** @brief Retourne vrai si la valeur stockée est déjà encodée. */
    static bool estEncode(const QString &valeur);

    Ui::ConfigForm  *ui;
    Configuration   *configuration;
    MySQLData       *m_db           = nullptr;
    QAction         *m_toggleAction = nullptr;
    bool             m_mdpVisible   = false;
};

#endif // CONFIGFORM_H
