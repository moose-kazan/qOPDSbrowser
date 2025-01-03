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
    //qDebug() << "DownloadHistory::DownloadHistory";
    historyList = new QList<DownloadHistoryItem>();
}

int DownloadHistory::rowCount(const QModelIndex &) const
{
    //qDebug() << "DownloadHistory::rowCount" << historyList->count();
    return historyList->count();
}

int DownloadHistory::columnCount(const QModelIndex &) const
{
    //qDebug() << "DownloadHistory::columnCount";
    return 3;
}

QVariant DownloadHistory::data( const QModelIndex &index, int role ) const
{
    QVariant value;
    switch (role)
    {
        case Qt::DisplayRole: //string
        {
            switch (index.column())
            {
                case COLUMN_FILENAME:
                {
                    value = stripFileName(historyList->at(index.row()).fileName);
                }
                break;

                case COLUMN_PROGRESS:
                {
                    value = prepareProgress(historyList->at(index.row()));
                }
                break;

                case COLUMN_STATUS:
                {
                    value = statusToString(historyList->at(index.row()).status);
                }
                break;
            }
        }
        break;

        case Qt::UserRole: //data
        {
            value = historyList->at(index.row()).fileName;
        }
        break;

        default:
        break;
    }

    //qDebug() << "DownloadHistory::data" << value;

    return value;
}

void DownloadHistory::populate(QList<DownloadHistoryItem> *newValues)
{
    //qDebug() << "DownloadHistory::populate";
    int idx = historyList->count();
    beginInsertRows(QModelIndex(), 1, idx);
    historyList = newValues;
    endInsertRows();
}

QVariant DownloadHistory::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "DownloadHistory::headerData" << section << orientation << role;
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COLUMN_FILENAME:
                return QString(tr("File name"));
            break;

            case COLUMN_PROGRESS:
                return QString(tr("Download progress"));
            break;

            case COLUMN_STATUS:
                return QString(tr("Download status"));
            break;
        }
    }
    return QVariant();
}

QByteArray DownloadHistory::HistoryItemAdd(QUrl url, QString fileName)
{
    DownloadHistoryItem newItem;

    newItem.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
    newItem.fileName = fileName;
    newItem.url = url;
    newItem.isNull = false;

    int newRow = historyList->count()+1;
    beginInsertRows(QModelIndex(), newRow, newRow);

    historyList->append(newItem);

    endInsertRows();

    return newItem.id;
}

DownloadHistoryItem DownloadHistory::HistoryItemGet(QByteArray id)
{
    for (int i = 0; i < historyList->length(); i++)
    {
        if (historyList->at(i).id == id)
        {
            return historyList->at(i);
        }
    }
    return DownloadHistoryItem();
}

void DownloadHistory::HistoryItemSuccess(QByteArray id)
{
    for (int i = 0; i < historyList->length(); i++)
    {
        if (historyList->at(i).id == id)
        {
            DownloadHistoryItem tmpItem = historyList->at(i);
            tmpItem.status = DownloadHistoryItem::downloadSuccess;
            historyList->replace(i, tmpItem);

            QModelIndex item_idx_s = this->index(i, 0);
            QModelIndex item_idx_e = this->index(i, this->columnCount(QModelIndex()));
            emit this->dataChanged(item_idx_s ,item_idx_e);

            return;
        }
    }
}

void DownloadHistory::HistoryItemFailed(QByteArray id)
{
    for (int i = 0; i < historyList->length(); i++)
    {
        if (historyList->at(i).id == id)
        {
            DownloadHistoryItem tmpItem = historyList->at(i);
            tmpItem.status = DownloadHistoryItem::downloadFailed;
            historyList->replace(i, tmpItem);

            QModelIndex item_idx_s = this->index(i, 0);
            QModelIndex item_idx_e = this->index(i, this->columnCount(QModelIndex()));
            emit this->dataChanged(item_idx_s ,item_idx_e);

            return;
        }
    }
}

void DownloadHistory::HistoryItemProgress(QByteArray id, qint64 bytesReceived, qint64 bytesTotal)
{
    for (int i = 0; i < historyList->length(); i++)
    {
        if (historyList->at(i).id == id)
        {
            DownloadHistoryItem tmpItem = historyList->at(i);
            tmpItem.status = DownloadHistoryItem::downloadProgress;
            tmpItem.bytesReceived = bytesReceived;
            tmpItem.bytesTotal = bytesTotal;
            historyList->replace(i, tmpItem);

            QModelIndex item_idx_s = this->index(i, 0);
            QModelIndex item_idx_e = this->index(i, this->columnCount(QModelIndex()));
            emit this->dataChanged(item_idx_s ,item_idx_e);

            return;
        }
    }
}

QString DownloadHistory::statusToString(DownloadHistoryItem::Status status) const
{
    QString statusName;
    switch (status)
    {
        case DownloadHistoryItem::downloadWaiting:
            statusName = tr("Waiting");
        break;

        case DownloadHistoryItem::downloadProgress:
            statusName = tr("Progress");
        break;

        case DownloadHistoryItem::downloadFailed:
            statusName = tr("Failed");
        break;

        case DownloadHistoryItem::downloadSuccess:
            statusName = tr("Success");
        break;
    }

    return statusName;
}

QString DownloadHistory::stripFileName(QString fileName) const
{
    const QFileInfo info(fileName);
    const QString baseName(info.fileName());
    return baseName;
}

QString DownloadHistory::prepareProgress(DownloadHistoryItem historyItem) const
{
    if (historyItem.status != DownloadHistoryItem::downloadProgress)
    {
        return "";
    }

    QString strReceived = prepareSize(historyItem.bytesReceived);
    QString strTotal = tr("unknown");
    if (historyItem.bytesTotal > 0)
    {
        strTotal = prepareSize(historyItem.bytesTotal);
    }

    return QString("%1/%2").arg(strReceived).arg(strTotal);
}

QString DownloadHistory::prepareSize(qint64 size) const
{
    if (size < 1024)
    {
        return QString("%1b").arg(size);
    }
    else if (size < 1024*1024)
    {
        return QString("%1Kb").arg(qint64(size/1024));
    }

    return QString("%1Kb").arg(qint64(size/(1024*1024)));
}
