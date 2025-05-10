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
    static const int COLUMN_SUMMARY = 1;

    explicit FeedParserViewModel(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void populate(const QList<FeedEntry>& newValues);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    FeedEntry at(int rowNum) const;

private:
    QList<FeedEntry> feedEntries;

};

#endif // FEEDPARSERVIEWMODEL_H
