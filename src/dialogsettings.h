#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QComboBox>
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
    void setData(QMap<QString, QString> uaVariants, QString uaSelected);
    QString getUserAgentName();

private:
    Ui::DialogSettings *ui;
    void showEvent(QShowEvent *event) override;

    QComboBox *userAgentList;

private slots:
    void actionOK();
    void actionCancel();
};

#endif // DIALOGSETTINGS_H
