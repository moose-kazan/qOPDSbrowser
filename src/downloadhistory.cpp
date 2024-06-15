#include "downloadhistory.h"

#include <QUuid>

DownloadHistory::DownloadHistory()
{

}

QByteArray DownloadHistory::Add(QUrl url, QString fileName)
{
    DownloadHistoryItem newItem;

    newItem.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
    newItem.fileName = fileName;
    newItem.url = url;
    newItem.isNull = false;

    historyList.append(newItem);

    return newItem.id;
}

DownloadHistoryItem DownloadHistory::Get(QByteArray id)
{
    for (int i = 0; i < historyList.length(); i++)
    {
        if (historyList.at(i).id == id)
        {
            return historyList.at(i);
        }
    }
    return DownloadHistoryItem();
}
