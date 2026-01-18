#ifndef BROWSERVIEWCONTEXTMENU_H
#define BROWSERVIEWCONTEXTMENU_H

#include <QObject>
#include <QMenu>

#include "feedparser.h"

class BrowserViewContextMenu : public QObject
{
    Q_OBJECT
public:
    explicit BrowserViewContextMenu(QWidget *parent = nullptr);
    QMenu *getMenu() const;
    void setData(FeedEntry feedEntry);

private:
    QMenu *contextMenu;
    QAction *itemCopyFeedLink;
    FeedEntry feedEntryData;

private slots:
    void actionCopyFeedLink();

};

#endif // BROWSERVIEWCONTEXTMENU_H
