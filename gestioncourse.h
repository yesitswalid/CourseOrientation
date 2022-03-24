#ifndef GESTIONCOURSE_H
#define GESTIONCOURSE_H

#include <QWidget>

namespace Ui {
class GestionCourse;
}

class GestionCourse : public QWidget
{
    Q_OBJECT

public:
    explicit GestionCourse(QWidget *parent = nullptr);
    ~GestionCourse();

private:
    Ui::GestionCourse *ui;
};

#endif // GESTIONCOURSE_H
