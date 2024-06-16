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
    QTime moment = QTime::currentTime();
    enum Status {downloadProgress, downloadSuccess, downloadFailed} status = downloadProgress;
};


class DownloadHistory : public QAbstractListModel
{
public:
    static const int COLUMN_FILENAME = 0;
    static const int COLUMN_STATUS = 1;

    DownloadHistory(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populate(QList<DownloadHistoryItem> *newValues);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QByteArray HistoryItemAdd(QUrl url, QString fileName);
    DownloadHistoryItem HistoryItemGet(QByteArray id);
    void HistoryItemSuccess(QByteArray id);
    void HistoryItemFailed(QByteArray id);

private:
    QList<DownloadHistoryItem> *historyList;
    QString statusToString(DownloadHistoryItem::Status status) const;
    QString stripFileName(QString fileName) const;
};

#endif // DOWNLOADHISTORY_H
