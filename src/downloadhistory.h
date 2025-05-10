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

    explicit DownloadHistory(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QByteArray HistoryItemAdd(const QUrl& url, const QString& fileName);
    DownloadHistoryItem HistoryItemGet(const QByteArray& id) const;
    DownloadHistoryItem HistoryItemGetByRow(int row) const;
    void HistoryItemSuccess(const QByteArray& id);
    void HistoryItemFailed(const QByteArray& id);
    void HistoryItemProgress(const QByteArray& id, qint64 bytesReceived, qint64 bytesTotal);

private:
    QList<DownloadHistoryItem> *historyList;
    static QString statusToString(DownloadHistoryItem::Status status) ;
    static QString stripFileName(const QString& fileName) ;
    QString prepareProgress(const DownloadHistoryItem& historyItem) const;
    static QString prepareSize(qint64 size) ;
};

#endif // DOWNLOADHISTORY_H
