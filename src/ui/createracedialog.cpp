#include "createracedialog.h"
#include "databasemanager.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QLabel>
#include <QFrame>

CreateRaceDialog::CreateRaceDialog(DatabaseManager *db, QWidget *parent)
    : QDialog(parent), m_db(db)
{
    setWindowTitle("Créer une nouvelle course");
    setMinimumWidth(440);
    setModal(true);

    /* ── Titre principal ─────────────────────────────────────────────────── */
    auto *titre = new QLabel("Nouvelle course", this);
    titre->setStyleSheet("font-size: 18px; font-weight: 600; color: #89b4fa; padding: 8px 0;");
    titre->setAlignment(Qt::AlignCenter);

    auto *separateur = new QFrame(this);
    separateur->setFrameShape(QFrame::HLine);
    separateur->setStyleSheet("color: #45475a;");

    /* ── Champs du formulaire ────────────────────────────────────────────── */
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Ex : Trail du Mont Blanc");

    m_dateEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(7), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");

    m_locationEdit = new QLineEdit(this);
    m_locationEdit->setPlaceholderText("Ex : Chamonix");

    m_longitudeEdit = new QLineEdit(this);
    m_longitudeEdit->setPlaceholderText("Ex : 6.8650");

    m_latitudeEdit = new QLineEdit(this);
    m_latitudeEdit->setPlaceholderText("Ex : 45.9237");

    m_difficultyEdit = new QSpinBox(this);
    m_difficultyEdit->setRange(1, 5);
    m_difficultyEdit->setValue(3);
    m_difficultyEdit->setSuffix("  / 5");

    m_typeEdit = new QComboBox(this);
    m_typeEdit->addItem("Course à pied",  0);
    m_typeEdit->addItem("VTT",            1);
    m_typeEdit->addItem("Multi-sport",    2);
    m_typeEdit->addItem("Trail",          3);
    m_typeEdit->addItem("Ski d'orientation", 4);

    m_bookEdit = new QSpinBox(this);
    m_bookEdit->setRange(1, 1000);
    m_bookEdit->setValue(50);
    m_bookEdit->setSuffix(" balises");

    m_departmentEdit = new QSpinBox(this);
    m_departmentEdit->setRange(1, 999);
    m_departmentEdit->setValue(74);
    m_departmentEdit->setPrefix("Dép. ");

    /* ── Layout en formulaire ────────────────────────────────────────────── */
    auto *form = new QFormLayout();
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(10);
    form->addRow("Nom de la course :",   m_nameEdit);
    form->addRow("Date et heure :",       m_dateEdit);
    form->addRow("Lieu :",                m_locationEdit);
    form->addRow("Longitude GPS :",       m_longitudeEdit);
    form->addRow("Latitude GPS :",        m_latitudeEdit);
    form->addRow("Difficulté :",          m_difficultyEdit);
    form->addRow("Type :",                m_typeEdit);
    form->addRow("Nombre de balises :",   m_bookEdit);
    form->addRow("Département :",         m_departmentEdit);

    /* ── Boutons OK / Annuler ────────────────────────────────────────────── */
    auto *btnBox = new QDialogButtonBox(this);
    auto *btnCreer  = btnBox->addButton("Créer la course", QDialogButtonBox::AcceptRole);
    auto *btnAnnul  = btnBox->addButton("Annuler",          QDialogButtonBox::RejectRole);
    btnCreer->setObjectName("buttonCreateRace");
    btnAnnul->setObjectName("buttonCancel");

    connect(btnBox, &QDialogButtonBox::accepted, this, &CreateRaceDialog::onCreate);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    /* ── Assemblage final ────────────────────────────────────────────────── */
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);
    layout->addWidget(titre);
    layout->addWidget(separateur);
    layout->addLayout(form);
    layout->addSpacing(8);
    layout->addWidget(btnBox);
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Calcule un nouvel ID unique pour la course (MAX(id) + 1)
 * ────────────────────────────────────────────────────────────────────────── */
int CreateRaceDialog::generateRaceId() const
{
    QSqlQuery q(m_db->getDb());
    if (q.exec("SELECT COALESCE(MAX(id), 0) + 1 FROM races") && q.next())
        return q.value(0).toInt();
    return 1;
}

/* ──────────────────────────────────────────────────────────────────────────
 *  Validation puis insertion en base
 * ────────────────────────────────────────────────────────────────────────── */
void CreateRaceDialog::onCreate()
{
    const QString nom    = m_nameEdit->text().trimmed();
    const QString lieu   = m_locationEdit->text().trimmed();

    /* ── Champs obligatoires ─────────────────────────────────────────────── */
    if (nom.isEmpty() || lieu.isEmpty()) {
        QMessageBox::warning(this, "Validation",
            "Le nom et le lieu de la course sont obligatoires.");
        return;
    }

    /* ── Vérifier qu'une course du même nom n'existe pas déjà ────────────── */
    if (m_db->isRaceExist(nom)) {
        QMessageBox::warning(this, "Doublon",
            "Une course nommée « " + nom + " » existe déjà.\n"
            "Veuillez choisir un autre nom.");
        return;
    }

    /* ── Validation GPS (optionnelle mais doit être numérique si renseigné) */
    const QString lon = m_longitudeEdit->text().trimmed();
    const QString lat = m_latitudeEdit->text().trimmed();
    bool okLon = true, okLat = true;
    if (!lon.isEmpty()) lon.toDouble(&okLon);
    if (!lat.isEmpty()) lat.toDouble(&okLat);
    if (!okLon || !okLat) {
        QMessageBox::warning(this, "Validation",
            "Les coordonnées GPS doivent être numériques (ex : 6.8650).");
        return;
    }

    /* ── Insertion en base ───────────────────────────────────────────────── */
    const int       raceId = generateRaceId();
    const int       deptId = m_departmentEdit->value();
    const QDateTime date   = m_dateEdit->dateTime();

    m_db->addRace(raceId, deptId, nom, date, lieu, lon, lat,
                  m_difficultyEdit->value(),
                  m_typeEdit->currentData().toInt(),
                  m_bookEdit->value());

    QMessageBox::information(this, "Course créée",
        "La course « " + nom + " » a été créée avec succès.\n"
        "Vous pouvez maintenant la sélectionner dans la liste.");

    emit raceCreated();
    accept();
}
