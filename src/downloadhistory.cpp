#include "downloadhistory.h"
#include "qfileinfo.h"

#include <QUuid>

/*
 * Some ideas found at: https://blog.altuninvv.ru/qt5/%D0%BC%D0%BE%D0%B4%D0%B5%D0%BB%D0%B8/65-%D1%81%D0%BE%D0%B7%D0%B4%D0%B0%D0%B5%D0%BC-%D0%BC%D0%BE%D0%B4%D0%B5%D0%BB%D1%8C-%D0%B4%D0%BB%D1%8F-qtableview-%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%B0-%D1%81-%D0%BC%D0%BE%D0%B4%D0%B5%D0%BB%D1%8F%D0%BC%D0%B8-%D0%B2-qt5-%D0%B4%D0%BB%D1%8F-%D0%BE%D1%82%D0%BE%D0%B1%D1%80%D0%B0%D0%B6%D0%B5%D0%BD%D0%B8%D1%8F-%D0%B4%D0%B0%D0%BD%D0%BD%D1%8B%D1%85-%D0%B2-%D0%B2%D0%B8%D0%B4%D0%B6%D0%B5%D1%82%D0%B0%D1%85
 *
 *
 */

DownloadHistory::DownloadHistory(QObject *parent) :QAbstractListModel(parent)
{
}

int DownloadHistory::rowCount(const QModelIndex &) const
{
    return historyList.count();
}

int DownloadHistory::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant DownloadHistory::data( const QModelIndex &index, int role ) const
{
    QVariant value;
    const DownloadHistoryItem &historyItem = historyList.at(index.row());
    switch (role)
    {
        case Qt::DisplayRole: //string
        {
            switch (index.column())
            {
                case COLUMN_FILENAME:
                {
                    value = stripFileName(historyItem.fileName);
                }
                break;

                case COLUMN_PROGRESS:
                {
                    value = prepareProgress(historyItem);
                }
                break;

                case COLUMN_STATUS:
                {
                    value = statusToString(historyItem.status);
                }
                break;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            value = historyItem.fileName;
        }
        break;

        default:
            break;
    }

    return value;
}

QVariant DownloadHistory::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COLUMN_FILENAME:
                return QString(tr("File name"));

            case COLUMN_PROGRESS:
                return QString(tr("Download progress"));

            case COLUMN_STATUS:
                return QString(tr("Download status"));
            default:
                break;
        }
    }
    return {};
}

QByteArray DownloadHistory::HistoryItemAdd(const QUrl& url, const QString& fileName)
{
    DownloadHistoryItem newItem;

    newItem.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
    newItem.fileName = fileName;
    newItem.url = url;
    newItem.isNull = false;

    int newRow = historyList.count();
    beginInsertRows(QModelIndex(), newRow, newRow);

    historyList.append(newItem);

    endInsertRows();

    return newItem.id;
}

DownloadHistoryItem DownloadHistory::HistoryItemGet(const QByteArray& id) const
{
    for (int i = 0; i < historyList.length(); i++)
    {
        if (historyList.at(i).id == id)
        {
            return historyList.at(i);
        }
    }
    return {};
}

DownloadHistoryItem DownloadHistory::HistoryItemGetByRow(const int row) const
{
    if (row < 0 || row >= historyList.count())
    {
        return {};
    }
    return historyList.at(row);
}

void DownloadHistory::HistoryItemSuccess(const QByteArray& id)
{
    updateItemStatus(id, DownloadHistoryItem::Status::downloadSuccess);
}

void DownloadHistory::HistoryItemFailed(const QByteArray& id)
{
    updateItemStatus(id, DownloadHistoryItem::Status::downloadFailed);
}

void DownloadHistory::HistoryItemProgress(const QByteArray& id, qint64 bytesReceived, const qint64 bytesTotal)
{
    updateItemStatus(id, DownloadHistoryItem::Status::downloadProgress, bytesReceived, bytesTotal);
}

void DownloadHistory::updateItemStatus(const QByteArray& id, DownloadHistoryItem::Status newStatus, qint64 bytesReceived, qint64 bytesTotal)
{
    for (int i = 0; i < historyList.length(); i++)
    {
        if (historyList.at(i).id == id)
        {
            DownloadHistoryItem tmpItem = historyList.at(i);
            tmpItem.status = newStatus;
            if (newStatus == DownloadHistoryItem::Status::downloadProgress)
            {
                tmpItem.bytesReceived = bytesReceived;
                tmpItem.bytesTotal = bytesTotal;
            }
            historyList.replace(i, tmpItem);

            QModelIndex item_idx_s = this->index(i, 0);
            QModelIndex item_idx_e = this->index(i, this->columnCount(QModelIndex()));
            emit this->dataChanged(item_idx_s, item_idx_e);

            return;
        }
    }
}

QString DownloadHistory::statusToString(const DownloadHistoryItem::Status status)
{
    QString statusName;
    switch (status)
    {
        case DownloadHistoryItem::Status::downloadWaiting:
            statusName = tr("Waiting");
        break;

        case DownloadHistoryItem::Status::downloadProgress:
            statusName = tr("Progress");
        break;

        case DownloadHistoryItem::Status::downloadFailed:
            statusName = tr("Failed");
        break;

        case DownloadHistoryItem::Status::downloadSuccess:
            statusName = tr("Success");
        break;
    }

    return statusName;
}

QString DownloadHistory::stripFileName(const QString& fileName)
{
    const QFileInfo info(fileName);
    const QString baseName(info.fileName());
    return baseName;
}

QString DownloadHistory::prepareProgress(const DownloadHistoryItem& historyItem) const
{
    if (historyItem.status != DownloadHistoryItem::Status::downloadProgress)
    {
        return "";
    }

    QString strReceived = prepareSize(historyItem.bytesReceived);
    QString strTotal = tr("unknown");
    if (historyItem.bytesTotal > 0)
    {
        strTotal = prepareSize(historyItem.bytesTotal);
    }

    return QString("%1/%2").arg(strReceived, strTotal);
}

QString DownloadHistory::prepareSize(const qint64 size)
{
    if (size < 1024)
    {
        return QString("%1b").arg(size);
    }
    else if (size < 1024*1024)
    {
        return QString("%1KB").arg(static_cast<qint64>(size / 1024));
    }

    return QString("%1MB").arg(static_cast<qint64>(size / (1024 * 1024)));
}
