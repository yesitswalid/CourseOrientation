#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <ui_configform.h>
#include <QWidget>

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm();
    ~ConfigForm();

private:
    Ui::ConfigForm ui;
};

#endif // CONFIGFORM_H
