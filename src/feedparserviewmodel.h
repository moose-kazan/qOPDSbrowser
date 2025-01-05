#ifndef FEEDPARSERVIEWMODEL_H
#define FEEDPARSERVIEWMODEL_H

#include "feedparser.h"

#include <QObject>
#include <QAbstractListModel>

class FeedParserViewModel : public QAbstractListModel
{
    Q_OBJECT
public:
    static const int COLUMN_TITLE = 0;

    FeedParserViewModel(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populate(QList<FeedEntry> newValues);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    FeedEntry at(int rowNum);

private:
    QList<FeedEntry> feedEntries;

};

#endif // FEEDPARSERVIEWMODEL_H
