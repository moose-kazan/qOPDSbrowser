#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);

    userAgentList = findChild<QComboBox *>("userAgentList");
    openFileAfterDownload = findChild<QCheckBox *>("openFileAfterDownload");
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::actionOK()
{
    accept();
}

void DialogSettings::actionCancel()
{
    reject();
}

void DialogSettings::showEvent(QShowEvent *event)
{
    (void)event;
}

void DialogSettings::setUaVariants(QMap<QString, QString> uaVariants, QString uaSelected)
{
    userAgentList->clear();
    for (int i = 0; i < uaVariants.keys().size(); i++)
    {
        userAgentList->addItem(uaVariants.keys().at(i));
    }
    userAgentList->setCurrentIndex(userAgentList->findText(uaSelected));
}

QString DialogSettings::getUserAgentName()
{
    return userAgentList->currentText();
}

void DialogSettings::setOpenAfterDownload(bool openAfterDownload)
{
    openFileAfterDownload->setChecked(openAfterDownload);
}

bool DialogSettings::getOpenAfterDownload()
{
    return openFileAfterDownload->isChecked();
}

