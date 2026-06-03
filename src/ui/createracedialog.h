#ifndef CREATERACEDIALOG_H
#define CREATERACEDIALOG_H

#include <QDialog>

class DatabaseManager;
class QLineEdit;
class QDateTimeEdit;
class QSpinBox;
class QComboBox;

/**
 * @class CreateRaceDialog
 * @brief Boîte de dialogue de création d'une nouvelle course.
 *
 * Permet à l'utilisateur de saisir manuellement les informations d'une course
 * (nom, date, lieu, GPS, difficulté, type, nombre de balises, département)
 * et de l'enregistrer directement dans la base SQLite locale.
 *
 * Émet le signal raceCreated() après insertion réussie pour que la fenêtre
 * principale puisse rafraîchir la liste déroulante des courses.
 */
class CreateRaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateRaceDialog(DatabaseManager *db, QWidget *parent = nullptr);
    ~CreateRaceDialog() override = default;

signals:
    /** Émis quand une course a été créée avec succès dans la base. */
    void raceCreated();

private slots:
    void onCreate();

private:
    /** Génère un nouvel ID unique en prenant MAX(id) + 1 dans la table races. */
    int generateRaceId() const;

    DatabaseManager *m_db = nullptr;

    QLineEdit     *m_nameEdit;
    QDateTimeEdit *m_dateEdit;
    QLineEdit     *m_locationEdit;
    QLineEdit     *m_longitudeEdit;
    QLineEdit     *m_latitudeEdit;
    QSpinBox      *m_difficultyEdit;
    QComboBox     *m_typeEdit;
    QSpinBox      *m_bookEdit;
    QSpinBox      *m_departmentEdit;
};

#endif // CREATERACEDIALOG_H
