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

QMenu *DownloadTableContextMenu::getMenu()
{
    return contextMenu;
}

void DownloadTableContextMenu::setData(bool downloaded, QString fileName, QUrl downloadLink)
{
    dataDownloaded = downloaded;
    dataFileName = fileName;
    dataDownloadLink = downloadLink;

    itemOpenFile->setEnabled(dataDownloaded);
}

void DownloadTableContextMenu::actionOpenFile()
{
    if (dataDownloaded)
    {
        QDesktopServices::openUrl(QUrl(dataFileName));
    }
}

void DownloadTableContextMenu::actionCopyLink()
{
    QGuiApplication::clipboard()->setText(dataDownloadLink.toString());
}
