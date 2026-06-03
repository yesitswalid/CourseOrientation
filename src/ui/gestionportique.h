#ifndef GESTIONPORTIQUE_H
#define GESTIONPORTIQUE_H

#include <QMainWindow>
#include <QString>

namespace Ui { class GestionPortique; }
class QComboBox;
class QPushButton;
class QLabel;

/**
 * @class GestionPortique
 * @brief Fenêtre de gestion du portique RFID et configuration du port série.
 *
 * Permet à l'utilisateur de :
 *   - Sélectionner le port série du portique physique (auto-détection des ports)
 *   - Changer le mode du lecteur UDP (RAZ / DATA)
 *
 * Émet le signal serialPortChanged() quand l'utilisateur sélectionne un autre
 * port, ce qui permet à GestionParticipant de reconnecter son QSerialPort.
 */
class GestionPortique : public QMainWindow
{
    Q_OBJECT

public:
    explicit GestionPortique(QWidget *parent = nullptr);
    ~GestionPortique() override;

    /** Retourne le nom du mode courant ("RAZ" ou "DATA") pour affichage. */
    QString getModeToString() const;

    /** Retourne le port série actuellement sélectionné par l'utilisateur. */
    QString getSelectedSerialPort() const;

signals:
    /** Émis lorsque l'utilisateur change le port série dans le combobox. */
    void serialPortChanged(const QString &portName);

private slots:
    void on_razButton_clicked();
    void on_dataButton_clicked();

    /** Rafraîchit la liste des ports série disponibles sur le système. */
    void onRefreshPorts();

    /** Émet serialPortChanged() quand l'utilisateur sélectionne un autre port. */
    void onPortSelectionChanged(int index);

private:
    /** Ajoute programmatiquement le sélecteur de port série au-dessus
     *  des boutons existants (RAZ / DATA). */
    void setupSerialPortSelector();

    Ui::GestionPortique *ui;

    /* Widgets du sélecteur de port (créés programmatiquement) */
    QComboBox   *m_portCombo    = nullptr;
    QPushButton *m_refreshButton = nullptr;
    QLabel      *m_portStatusLabel = nullptr;
};

#endif // GESTIONPORTIQUE_H
