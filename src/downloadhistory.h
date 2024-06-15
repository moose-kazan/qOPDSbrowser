#ifndef DOWNLOADHISTORY_H
#define DOWNLOADHISTORY_H

#include <QByteArray>
#include <QUrl>
#include <QList>

struct DownloadHistoryItem
{
    bool isNull = true;
    QByteArray id;
    QUrl url;
    QString fileName;
};


class DownloadHistory
{
public:
    DownloadHistory();
    QByteArray Add(QUrl url, QString fileName);
    DownloadHistoryItem Get(QByteArray id);

private:
    QList<DownloadHistoryItem> historyList;
};

#endif // DOWNLOADHISTORY_H
