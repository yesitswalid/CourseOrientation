#include "gestioncourse.h"
#include "ui_gestioncourse.h"

GestionCourse::GestionCourse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GestionCourse)
{
    ui->setupUi(this);
}

GestionCourse::~GestionCourse()
{
    delete ui;
}
