#ifndef DOWNLOADHISTORY_H
#define DOWNLOADHISTORY_H

#include <QByteArray>
#include <QUrl>
#include <QList>
#include <QTime>
#include <QAbstractListModel>

struct DownloadHistoryItem
{
    bool isNull = true;
    QByteArray id;
    QUrl url;
    QString fileName;
    qint64 bytesReceived;
    qint64 bytesTotal;
    QTime moment = QTime::currentTime();
    enum Status {downloadWaiting, downloadProgress, downloadSuccess, downloadFailed} status = downloadWaiting;
};


class DownloadHistory : public QAbstractListModel
{
    Q_OBJECT
public:
    static const int COLUMN_FILENAME = 0;
    static const int COLUMN_STATUS   = 1;
    static const int COLUMN_PROGRESS = 2;

    DownloadHistory(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QByteArray HistoryItemAdd(QUrl url, QString fileName);
    DownloadHistoryItem HistoryItemGet(QByteArray id);
    DownloadHistoryItem HistoryItemGetByRow(int row);
    void HistoryItemSuccess(QByteArray id);
    void HistoryItemFailed(QByteArray id);
    void HistoryItemProgress(QByteArray id, qint64 bytesReceived, qint64 bytesTotal);

private:
    QList<DownloadHistoryItem> *historyList;
    QString statusToString(DownloadHistoryItem::Status status) const;
    QString stripFileName(QString fileName) const;
    QString prepareProgress(DownloadHistoryItem historyItem) const;
    QString prepareSize(qint64 size) const;
};

#endif // DOWNLOADHISTORY_H
