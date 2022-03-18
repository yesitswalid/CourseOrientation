#include "inscription.h"
#include "ui_inscription.h"

inscription::inscription(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inscription)
{
    ui->setupUi(this);
}

inscription::~inscription()
{
    delete ui;
}
