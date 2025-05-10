#include "feedparserviewmodel.h"

#include "icons.h"

#include <QIcon>
#include <QTextDocumentFragment>

FeedParserViewModel::FeedParserViewModel(QObject *parent) : QAbstractListModel(parent)
{

}

int FeedParserViewModel::rowCount(const QModelIndex &) const
{
    return feedEntries.count();
}

int FeedParserViewModel::columnCount(const QModelIndex &) const
{
    return 2;
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

                case COLUMN_SUMMARY:
                    if (feedEntries.at(index.row()).summary.type == FeedEntry::Summary::text)
                    {
                        value = feedEntries.at(index.row()).summary.value;
                    }
                    else if (feedEntries.at(index.row()).summary.type == FeedEntry::Summary::html)
                    {
                        value = QTextDocumentFragment::fromHtml(feedEntries.at(index.row()).summary.value).toPlainText();
                    }
                break;
            default: ;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    value = feedEntries.at(index.row()).title;
                }
                break;

                case COLUMN_SUMMARY:
                if (feedEntries.at(index.row()).summary.type == FeedEntry::Summary::text)
                {
                    value = feedEntries.at(index.row()).summary.value;
                }
                break;
            default: ;
            }
        }
        break;

        case Qt::DecorationRole:
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    switch (feedEntries.at(index.row()).entryType)
                    {
                        case FeedEntry::feed:
                        value = QIcon::fromTheme("folder-remote", QIcon(icons::folderRemote));
                        break;

                        case FeedEntry::book:
                            value = QIcon::fromTheme("text-x-generic", QIcon(icons::textXGeneric));
                        break;

                        default:
                        break;
                    }
                }
            default: ;
            }
        }
        break;

        case Qt::ToolTipRole:
        {
            switch (index.column())
            {
            case COLUMN_TITLE:
            {
                value = feedEntries.at(index.row()).title;
            }
            break;

            case COLUMN_SUMMARY:
                if (feedEntries.at(index.row()).summary.type == FeedEntry::Summary::text)
                {
                    value = feedEntries.at(index.row()).summary.value;
                }
                break;
        default: ;
            }
        }
        break;

        default:
        break;
    }

    return value;
}

void FeedParserViewModel::populate(const QList<FeedEntry>& newValues)
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

            case COLUMN_SUMMARY:
                return QString(tr("Summary"));

            default: ;
        }
    }
    return {};
}


FeedEntry FeedParserViewModel::at(const int rowNum) const
{
    return feedEntries.at(rowNum);
}
