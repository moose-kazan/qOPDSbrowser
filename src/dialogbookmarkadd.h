#ifndef DIALOGBOOKMARKADD_H
#define DIALOGBOOKMARKADD_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class DialogBookmarkAdd;
}

class DialogBookmarkAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBookmarkAdd(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~DialogBookmarkAdd();

    QString bookmarkTitle;
    QString bookmarkUrl;

private:
    Ui::DialogBookmarkAdd *ui;

    QLineEdit* editTitle;
    QLineEdit* editUrl;

    void showEvent(QShowEvent *event) override;

private slots:
    void actionOK();
    void actionCancel();
};

#endif // DIALOGBOOKMARKADD_H
