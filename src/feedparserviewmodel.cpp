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
    const FeedEntry &feedEntry = feedEntries.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole: //string
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    value = feedEntry.title;
                }
                break;

                case COLUMN_SUMMARY:
                    if (feedEntry.summary.type == FeedEntry::Summary::Type::text)
                    {
                        value = feedEntry.summary.value;
                    }
                    else if (feedEntry.summary.type == FeedEntry::Summary::Type::html)
                    {
                        value = QTextDocumentFragment::fromHtml(feedEntry.summary.value).toPlainText();
                    }
                break;
            default:
                break;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    value = feedEntry.title;
                }
                break;

                case COLUMN_SUMMARY:
                if (feedEntry.summary.type == FeedEntry::Summary::Type::text)
                {
                    value = feedEntry.summary.value;
                }
                break;
            default:
                break;
            }
        }
        break;

        case Qt::DecorationRole:
        {
            switch (index.column())
            {
                case COLUMN_TITLE:
                {
                    switch (feedEntry.entryType)
                    {
                        case FeedEntry::EntryType::feed:
                        value = QIcon::fromTheme("folder-remote", QIcon(icons::folderRemote));
                        break;

                        case FeedEntry::EntryType::book:
                            value = QIcon::fromTheme("text-x-generic", QIcon(icons::textXGeneric));
                        break;

                        default:
                            break;
                    }
                }
            default:
                break;
            }
        }
        break;

        case Qt::ToolTipRole:
        {
            switch (index.column())
            {
            case COLUMN_TITLE:
            {
                value = feedEntry.title;
            }
            break;

            case COLUMN_SUMMARY:
                if (feedEntry.summary.type == FeedEntry::Summary::Type::text)
                {
                    value = feedEntry.summary.value;
                }
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

void FeedParserViewModel::populate(const QList<FeedEntry>& newValues)
{
    beginResetModel();
    feedEntries = newValues;
    endResetModel();
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
