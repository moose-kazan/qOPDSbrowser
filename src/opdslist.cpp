#include "opdslist.h"

#include <QUuid>

OPDSList::OPDSList()
{

}

QSettings *OPDSList::cfg = nullptr;

void OPDSList::add(QString url, QString title)
{
    OPDSFeedBookmarks bookmarksList = list();

    OPDSFeedBookmark newBookmark;
    newBookmark.title = title;
    newBookmark.url = url;
    newBookmark.id = QUuid::createUuid().toString();

    bookmarksList.bookmarks.append(newBookmark);

    save(bookmarksList);
}

void OPDSList::remove(QString Id)
{
    OPDSFeedBookmarks bookmarksList = list();


    int pos = -1;
    for (int i = 0; i < bookmarksList.bookmarks.count(); i++)
    {
        if (bookmarksList.bookmarks.at(i).id == Id)
        {
            pos = i;
            break;
        }
    }

    if (pos != -1)
    {
        bookmarksList.bookmarks.removeAt(pos);
    }

    save(bookmarksList);
}

void OPDSList::update(QString Id, QString url, QString title)
{
    OPDSFeedBookmarks bookmarksList = list();

    for (int i = 0; i < bookmarksList.bookmarks.count(); i++)
    {
        if (bookmarksList.bookmarks.at(i).id == Id)
        {
            bookmarksList.bookmarks[i].title = title;
            bookmarksList.bookmarks[i].url = url;
            break;
        }
    }

    save(bookmarksList);
}

OPDSFeedBookmarks OPDSList::list()
{
    OPDSFeedBookmarks bookmarksList;

    int size = getCfg()->beginReadArray("bookmarks");

    for (int i = 0; i < size; i++)
    {
        getCfg()->setArrayIndex(i);
        OPDSFeedBookmark bookmark;
        bookmark.url = getCfg()->value("url", "").toString();
        bookmark.title = getCfg()->value("title", "").toString();
        bookmark.id = getCfg()->value("id", "").toString();

        if (bookmark.url != "" && bookmark.title != "")
        {
            bookmarksList.bookmarks.append(bookmark);
        }
    }

    getCfg()->endArray();

    for (int i = 0; i < bookmarksList.bookmarks.count()-1; i++)
    {
        for (int j = i+1; j < bookmarksList.bookmarks.count(); j++)
        {
            OPDSFeedBookmark sI = bookmarksList.bookmarks[i];
            OPDSFeedBookmark sJ = bookmarksList.bookmarks[j];

            if (sI.title.compare(sJ.title, Qt::CaseInsensitive) > 0)
            {
                bookmarksList.bookmarks[i] = sJ;
                bookmarksList.bookmarks[j] = sI;
            }
        }
    }

    return bookmarksList;
}

void OPDSList::save(OPDSFeedBookmarks data)
{
    getCfg()->beginWriteArray("bookmarks");

    for (int i = 0; i < data.bookmarks.count(); i++)
    {
        getCfg()->setArrayIndex(i);
        getCfg()->setValue("url", data.bookmarks.at(i).url);
        getCfg()->setValue("title", data.bookmarks.at(i).title);
        getCfg()->setValue("id", data.bookmarks.at(i).id);
    }
    getCfg()->endArray();
}

QSettings* OPDSList::getCfg()
{
    if (cfg == nullptr) {
      cfg = new QSettings("YLSoftware", "qOPDSbrowser");
    }
    return cfg;
}
