#include "opdslist.h"

#include <QUuid>

OPDSList::OPDSList(QObject *parent) : QAbstractListModel(parent)
{
    load();
}

void OPDSList::add(const QString& url, const QString& title)
{
    if (url == "" || title == "")
    {
        return;
    }

    OPDSFeedBookmark newBookmark;

    newBookmark.title = title;
    newBookmark.url = url;
    newBookmark.id = QUuid::createUuid().toString();

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i).title.compare(title, Qt::CaseInsensitive) > 0)
        {
            beginInsertRows(QModelIndex(), i, i);
            bookmarksList.insert(i, newBookmark);
            endInsertRows();

            save();

            return;
        }
    }

    int newRow = bookmarksList.count();
    beginInsertRows(QModelIndex(), newRow, newRow);
    bookmarksList.append(newBookmark);
    endInsertRows();

    save();
}

void OPDSList::remove(const QString& Id)
{
    int pos = -1;
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i).id == Id)
        {
            pos = i;
            break;
        }
    }

    if (pos != -1)
    {
        beginRemoveRows(QModelIndex(), pos, pos);
        bookmarksList.removeAt(pos);
        endRemoveRows();
    }

    save();
}

void OPDSList::update(const QString& Id, const QString& url, const QString& title)
{
    if (url == "" || title == "")
    {
        return;
    }

    int indexOld = -1;
    int indexNew = -1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i).id == Id)
        {
            indexOld = i;

            bookmarksList[indexOld].title = title;
            bookmarksList[indexOld].url = url;

            break;
        }
    }

    if (indexOld == -1)
    {
        return;
    }

    QList<OPDSFeedBookmark> bookmarksListTmp = bookmarksList;
    for (int i = 0; i + 1 < bookmarksListTmp.count(); i++)
    {
        for (int j = i + 1; j < bookmarksListTmp.count(); j++)
        {
            if (bookmarksListTmp[i].title.compare(bookmarksListTmp[j].title, Qt::CaseInsensitive) > 0)
            {
                bookmarksListTmp.swapItemsAt(i, j);
            }
        }
    }
    for (int i = 0; i < bookmarksListTmp.count(); i++)
    {
        if (bookmarksListTmp.at(i).id == Id)
        {
            indexNew = i;
            break;
        }
    }

    if (indexOld != indexNew)
    {
        beginMoveRows(QModelIndex(), indexOld, indexOld, QModelIndex(), indexNew);
        bookmarksList.move(indexOld, indexNew);
        endMoveRows();
        emit dataChanged(createIndex(indexNew, 0), createIndex(indexNew, 0));
    }
    else
    {
        emit dataChanged(createIndex(indexOld, 0), createIndex(indexOld, 0));
    }

    save();
}

OPDSFeedBookmark OPDSList::at(int rowNum) const
{
    return bookmarksList.at(rowNum);
}

void OPDSList::load()
{
    bookmarksList.clear();

    int size = getCfg().beginReadArray("bookmarks");

    for (int i = 0; i < size; i++)
    {
        getCfg().setArrayIndex(i);
        OPDSFeedBookmark bookmark;
        bookmark.url = getCfg().value("url", "").toString();
        bookmark.title = getCfg().value("title", "").toString();
        bookmark.id = getCfg().value("id", "").toString();

        if (bookmark.url != "" && bookmark.title != "")
        {
            bookmarksList.append(bookmark);
        }
    }

    getCfg().endArray();

    for (int i = 0; i + 1 < bookmarksList.count(); i++)
    {
        for (int j = i + 1; j < bookmarksList.count(); j++)
        {
            if (bookmarksList[i].title.compare(bookmarksList[j].title, Qt::CaseInsensitive) > 0)
            {
                bookmarksList.swapItemsAt(i, j);
            }
        }
    }
}

void OPDSList::save() const
{
    getCfg().beginWriteArray("bookmarks");

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        getCfg().setArrayIndex(i);
        getCfg().setValue("url", bookmarksList.at(i).url);
        getCfg().setValue("title", bookmarksList.at(i).title);
        getCfg().setValue("id", bookmarksList.at(i).id);
    }
    getCfg().endArray();
}

QSettings& OPDSList::getCfg()
{
    static QSettings instance("YLSoftware", "qOPDSbrowser");
    return instance;
}


int OPDSList::rowCount(const QModelIndex &) const
{
    return bookmarksList.count();
}

int OPDSList::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant OPDSList::data(const QModelIndex &index, const int role) const
{
    QVariant value;
    const OPDSFeedBookmark &bookmark = bookmarksList.at(index.row());
    switch (role)
    {
        case Qt::DisplayRole: //string
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    value = bookmark.title;
                }
                break;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            value = bookmark.title;
        }
        break;

        default:
            break;
    }

    return value;

}


QVariant OPDSList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COLUMN_TITLE:
                return QString(tr("Bookmark title"));
            default:
                break;
        }
    }
    return {};
}
