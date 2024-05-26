#include "dialogbookmarkadd.h"
#include "ui_dialogbookmarkadd.h"

DialogBookmarkAdd::DialogBookmarkAdd(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::DialogBookmarkAdd)
{
    ui->setupUi(this);
    setWindowTitle(tr("New bookmark"));
    setModal(true);
    setFixedSize(size());

    editTitle = findChild<QLineEdit *>("titleEdit");
    editUrl = findChild<QLineEdit *>("urlEdit");
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

    if (bookmarkUrl == "" || !QUrl(bookmarkUrl).isValid())
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

    editTitle->setText("");
    editUrl->setText("");

    bookmarkTitle = "";
    bookmarkUrl = "";
}
