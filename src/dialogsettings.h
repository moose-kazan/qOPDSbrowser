#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QStandardItemModel>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~DialogSettings();
    void setUaVariants(QMap<QString, QString> uaVariants, QString uaSelected);
    QString getUserAgentName();

    void setOpenAfterDownload(bool openAfterDownload);
    bool getOpenAfterDownload();

    QString getDefaultSaveDirectory();
    void setDefaultSaveDirectory(QString dir);

private:
    Ui::DialogSettings *ui;
    void showEvent(QShowEvent *event) override;

    QComboBox *userAgentList;
    QCheckBox *openFileAfterDownload;
    QLineEdit *defaultDownloadDirectory;
    QFileDialog *defaultDownloadDirectoryDialog;

private slots:
    void actionOK();
    void actionCancel();
    void actionSelectDefaultDownloadDirectory();
};

#endif // DIALOGSETTINGS_H
