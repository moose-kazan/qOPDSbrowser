#include "dialogbookmarkedit.h"
#include "ui_dialogbookmarkedit.h"

DialogBookmarkEdit::DialogBookmarkEdit(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DialogBookmarkEdit)
{
    ui->setupUi(this);
    setWindowTitle(tr("Edit bookmark"));
    setModal(true);
    setFixedSize(size());

    editTitle = findChild<QLineEdit *>("titleEdit");
    editUrl = findChild<QLineEdit *>("urlEdit");
}

DialogBookmarkEdit::~DialogBookmarkEdit()
{
    delete ui;
}

void DialogBookmarkEdit::actionOK()
{
    bookmarkTitle = editTitle->text();
    bookmarkUrl = editUrl->text();

    if (bookmarkTitle == "")
    {
        return;
    }

    if (bookmarkUrl == "" || !QUrl(bookmarkUrl).isValid())
    {
        return;
    }

    accept();
}

void DialogBookmarkEdit::actionCancel()
{
    reject();
}

void DialogBookmarkEdit::showEvent(QShowEvent *event)
{
    (void)event;

    editTitle->setText(bookmarkTitle);
    editUrl->setText(bookmarkUrl);
}
