#include "feedparserviewmodel.h"
#include "qicon.h"

FeedParserViewModel::FeedParserViewModel(QObject *parent) : QAbstractListModel(parent)
{

}

int FeedParserViewModel::rowCount(const QModelIndex &) const
{
    return feedEntries.count();
}

int FeedParserViewModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant FeedParserViewModel::data(const QModelIndex &index, int role) const
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
                    value = feedEntries.at(index.row()).title;
                }
                break;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            value = feedEntries.at(index.row()).title;
        }
        break;

        case Qt::DecorationRole:
        {
            switch (feedEntries.at(index.row()).entryType)
            {
                case FeedEntry::feed:
                    value = QIcon::fromTheme("folder-remote");
                break;

                case FeedEntry::book:
                    value = QIcon::fromTheme("text-x-generic");
                break;

                default:
                break;
            }
        }
        break;

        default:
        break;
    }

    return value;
}

void FeedParserViewModel::populate(QList<FeedEntry> newValues)
{
    beginResetModel();
    feedEntries.clear();
    endResetModel();

    int idx = feedEntries.count();
    beginInsertRows(QModelIndex(), 1, idx);
    feedEntries = newValues;
    endInsertRows();
}

QVariant FeedParserViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COLUMN_TITLE:
                return QString(tr("Title"));
            break;
        }
    }
    return QVariant();
}


FeedEntry FeedParserViewModel::at(int rowNum)
{
    return feedEntries.at(rowNum);
}
