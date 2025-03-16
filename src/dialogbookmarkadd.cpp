#include "dialogbookmarkadd.h"
#include "ui_dialogbookmarkadd.h"

#include "urlvalidator.h"

#include <QClipboard>
#include <QGuiApplication>


DialogBookmarkAdd::DialogBookmarkAdd(QWidget *parent, Qt::WindowFlags f) :
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

    editTitle->setText("");
    editUrl->setText("");
    editUrl->insert(clipboardText);

    bookmarkTitle = "";
    bookmarkUrl = editUrl->text();
}
