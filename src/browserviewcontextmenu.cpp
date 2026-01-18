#include "browserviewcontextmenu.h"

#include <QGuiApplication>
#include <QClipboard>

BrowserViewContextMenu::BrowserViewContextMenu(QWidget *parent)
{
    contextMenu = new QMenu(parent);

    itemCopyFeedLink = new QAction(tr("Copy feed link"), parent);

    connect(itemCopyFeedLink, SIGNAL(triggered()), this, SLOT(actionCopyFeedLink()));

    contextMenu->addAction(itemCopyFeedLink);
}


void BrowserViewContextMenu::setData(FeedEntry feedEntry)
{
    feedEntryData = feedEntry;

    itemCopyFeedLink->setEnabled(feedEntryData.entryType == FeedEntry::feed);
}

void BrowserViewContextMenu::actionCopyFeedLink()
{
    qDebug() << feedEntryData.links.at(0).link;
    if (feedEntryData.links.count() > 0)
    {
        QGuiApplication::clipboard()->setText(feedEntryData.links.at(0).link);
    }

}

QMenu *BrowserViewContextMenu::getMenu() const
{
    return contextMenu;
}

