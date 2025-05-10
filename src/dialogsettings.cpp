#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent, const Qt::WindowFlags f) :
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

void DialogSettings::actionSelectDefaultDownloadDirectory() const
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

void DialogSettings::setUaVariants(const QMap<QString, QString>& uaVariants, const QString& uaSelected) const
{
    userAgentList->clear();
    for (int i = 0; i < uaVariants.size(); i++)
    {
        userAgentList->addItem(uaVariants.keys().at(i));
    }
    userAgentList->setCurrentIndex(userAgentList->findText(uaSelected));
}

QString DialogSettings::getUserAgentName() const
{
    return userAgentList->currentText();
}

void DialogSettings::setOpenAfterDownload(bool openAfterDownload) const
{
    openFileAfterDownload->setChecked(openAfterDownload);
}

bool DialogSettings::getOpenAfterDownload() const
{
    return openFileAfterDownload->isChecked();
}

QString DialogSettings::getDefaultSaveDirectory() const
{
    return defaultDownloadDirectory->text();
}

void DialogSettings::setDefaultSaveDirectory(const QString& dir) const
{
    defaultDownloadDirectory->setText(dir);
}
