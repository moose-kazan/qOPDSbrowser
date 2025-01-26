#include "opdslist.h"

#include <QUuid>

QSettings *OPDSList::cfg = nullptr;

OPDSList::OPDSList(QObject *parent) : QAbstractListModel(parent)
{
    load();
}

void OPDSList::add(QString url, QString title)
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
        if (bookmarksList.at(i).title > newBookmark.title)
        {
            beginInsertRows(QModelIndex(), i, i);
            bookmarksList.insert(i, newBookmark);
            endInsertRows();

            save();

            return;
        }
    }

    int newRow = bookmarksList.count()+1;
    beginInsertRows(QModelIndex(), newRow, newRow);
    bookmarksList.append(newBookmark);
    endInsertRows();

    save();
}

void OPDSList::remove(QString Id)
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

void OPDSList::update(QString Id, QString url, QString title)
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
        }
        else if (indexNew == -1 && bookmarksList.at(i).title > title)
        {
            indexNew = i;
        }
    }

    if (indexOld != -1)
    {
        bookmarksList[indexOld].title = title;
        bookmarksList[indexOld].url = url;
        dataChanged(createIndex(indexOld, 0), createIndex(indexOld,0));
    }

    if (indexOld != -1)
    {
        if (indexNew == -1)
        {
            indexNew = bookmarksList.count()-1;
        }

        beginMoveRows(QModelIndex(), indexOld, indexOld, QModelIndex(), indexNew);
        bookmarksList.move(indexOld, indexNew);
        endMoveRows();
    }

    save();
}

OPDSFeedBookmark OPDSList::at(int rowNum)
{
    return bookmarksList.at(rowNum);
}

void OPDSList::load()
{
    bookmarksList.clear();

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
            bookmarksList.append(bookmark);
        }
    }

    getCfg()->endArray();

    for (int i = 0; i < bookmarksList.count()-1; i++)
    {
        for (int j = i+1; j < bookmarksList.count(); j++)
        {
            OPDSFeedBookmark sI = bookmarksList[i];
            OPDSFeedBookmark sJ = bookmarksList[j];

            if (sI.title.compare(sJ.title, Qt::CaseInsensitive) > 0)
            {
                bookmarksList[i] = sJ;
                bookmarksList[j] = sI;
            }
        }
    }
}

void OPDSList::save()
{
    getCfg()->beginWriteArray("bookmarks");

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        getCfg()->setArrayIndex(i);
        getCfg()->setValue("url", bookmarksList.at(i).url);
        getCfg()->setValue("title", bookmarksList.at(i).title);
        getCfg()->setValue("id", bookmarksList.at(i).id);
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


int OPDSList::rowCount(const QModelIndex &) const
{
    return bookmarksList.count();
}

int OPDSList::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant OPDSList::data(const QModelIndex &index, int role) const
{
    QVariant value;
    switch (role)
    {
        case Qt::DisplayRole: //string
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    value = bookmarksList.at(index.row()).title;
                }
                break;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            value = bookmarksList.at(index.row()).title;
        }
        break;

        default:
        break;
    }

    //qDebug() << "DownloadHistory::data" << value;

    return value;

}


QVariant OPDSList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COLUMN_TITLE:
                return QString(tr("Bookmark title"));
            break;
        }
    }
    return QVariant();
};
