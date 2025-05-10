#include "dialogbookmarkedit.h"
#include "ui_dialogbookmarkedit.h"

#include "urlvalidator.h"

DialogBookmarkEdit::DialogBookmarkEdit(QWidget *parent, const Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DialogBookmarkEdit)
{
    ui->setupUi(this);
    setWindowTitle(tr("Edit bookmark"));
    setModal(true);
    setFixedSize(size());

    editTitle = findChild<QLineEdit *>("titleEdit");
    editUrl = findChild<QLineEdit *>("urlEdit");
    editUrl->setValidator(new UrlValidator(this));
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

    if (!editUrl->hasAcceptableInput() || !QUrl(bookmarkUrl).isValid())
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
