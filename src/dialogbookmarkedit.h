#ifndef DIALOGBOOKMARKEDIT_H
#define DIALOGBOOKMARKEDIT_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class DialogBookmarkEdit;
}

class DialogBookmarkEdit : public QDialog
{
    Q_OBJECT

public:
    DialogBookmarkEdit(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~DialogBookmarkEdit();

    QString bookmarkTitle;
    QString bookmarkUrl;
    QString bookmarkId;

private:
    Ui::DialogBookmarkEdit *ui;

    QLineEdit* editTitle;
    QLineEdit* editUrl;

    void showEvent(QShowEvent *event) override;

private slots:
    void actionOK();
    void actionCancel();
};

#endif // DIALOGBOOKMARKEDIT_H
