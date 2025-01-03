#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);

    userAgentList = findChild<QComboBox *>("userAgentList");
    openFileAfterDownload = findChild<QCheckBox *>("openFileAfterDownload");
    defaultDownloadDirectory = findChild<QLineEdit *>("defaultDownloadDirectoryEdit");

    defaultDownloadDirectoryDialog = new QFileDialog(this);
    defaultDownloadDirectoryDialog->setFileMode(QFileDialog::Directory);
    defaultDownloadDirectoryDialog->setOption(QFileDialog::ShowDirsOnly, true);
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

void DialogSettings::actionSelectDefaultDownloadDirectory()
{
    defaultDownloadDirectoryDialog->setDirectory(getDefaultSaveDirectory());
    if (defaultDownloadDirectoryDialog->exec() == QDialog::Accepted)
    {
        setDefaultSaveDirectory(defaultDownloadDirectoryDialog->directory().absolutePath());
    }
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

QString DialogSettings::getDefaultSaveDirectory()
{
    return defaultDownloadDirectory->text();
}

void DialogSettings::setDefaultSaveDirectory(QString dir)
{
    defaultDownloadDirectory->setText(dir);
}
