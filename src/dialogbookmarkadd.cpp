#include "dialogbookmarkadd.h"
#include "ui_dialogbookmarkadd.h"

#include "urlvalidator.h"

#include <QClipboard>
#include <QGuiApplication>


DialogBookmarkAdd::DialogBookmarkAdd(QWidget *parent, const Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DialogBookmarkAdd)
{
    ui->setupUi(this);
    setWindowTitle(tr("New bookmark"));
    setModal(true);
    setFixedSize(size());

    editTitle = findChild<QLineEdit *>("titleEdit");
    editTitle->setPlaceholderText(tr("Bookmark title"));

    editUrl = findChild<QLineEdit *>("urlEdit");
    editUrl->setPlaceholderText("https://");
    editUrl->setValidator(new UrlValidator(this));
}

DialogBookmarkAdd::~DialogBookmarkAdd()
{
    delete ui;
}

void DialogBookmarkAdd::actionOK()
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

void DialogBookmarkAdd::actionCancel()
{
    reject();
}

void DialogBookmarkAdd::showEvent(QShowEvent *event)
{
    (void)event;

    QClipboard *clipboard = QGuiApplication::clipboard();
    QString clipboardText = clipboard->text();

    editTitle->clear();
    editUrl->clear();
    editUrl->insert(clipboardText);
    if (editUrl->text().length() == 0)
    {
        editUrl->insert(bookmarkUrl);
        editTitle->setText(bookmarkTitle);
    }


    bookmarkTitle = "";
    bookmarkUrl = editUrl->text();
}
