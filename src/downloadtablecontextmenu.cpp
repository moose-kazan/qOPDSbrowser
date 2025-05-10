#include "downloadtablecontextmenu.h"

#include <QDesktopServices>
#include <QGuiApplication>
#include <QClipboard>

DownloadTableContextMenu::DownloadTableContextMenu(QWidget *parent) {
    contextMenu = new QMenu(parent);

    itemOpenFile = new QAction(tr("Open file"), parent);
    itemCopyLink = new QAction(tr("Copy link"), parent);

    connect(itemOpenFile, SIGNAL(triggered()), this, SLOT(actionOpenFile()));
    connect(itemCopyLink, SIGNAL(triggered()), this, SLOT(actionCopyLink()));

    contextMenu->addAction(itemOpenFile);
    contextMenu->addAction(itemCopyLink);
}

QMenu *DownloadTableContextMenu::getMenu() const
{
    return contextMenu;
}

void DownloadTableContextMenu::setData(const bool downloaded, const QString& fileName, const QUrl& downloadLink)
{
    dataDownloaded = downloaded;
    dataFileName = fileName;
    dataDownloadLink = downloadLink;

    itemOpenFile->setEnabled(dataDownloaded);
}

void DownloadTableContextMenu::actionOpenFile() const
{
    if (dataDownloaded)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(dataFileName));
    }
}

void DownloadTableContextMenu::actionCopyLink() const
{
    QGuiApplication::clipboard()->setText(dataDownloadLink.toString());
}
