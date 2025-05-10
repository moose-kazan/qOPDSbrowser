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
    void setUaVariants(const QMap<QString, QString>& uaVariants, const QString& uaSelected) const;
    QString getUserAgentName() const;

    void setOpenAfterDownload(bool openAfterDownload) const;
    bool getOpenAfterDownload() const;

    QString getDefaultSaveDirectory() const;
    void setDefaultSaveDirectory(const QString& dir) const;

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
    void actionSelectDefaultDownloadDirectory() const;
};

#endif // DIALOGSETTINGS_H
