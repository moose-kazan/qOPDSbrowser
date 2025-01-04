#ifndef OPDSLIST_H
#define OPDSLIST_H

#include <QString>
#include <QSettings>
#include <QAbstractListModel>

struct OPDSFeedBookmark
{
    QString id;
    QString url;
    QString title;
};

class OPDSList : public QAbstractListModel
{
    Q_OBJECT
public:
    static const int COLUMN_TITLE = 0;

    OPDSList(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populate(QList<OPDSFeedBookmark> *newValues);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    void add(QString url, QString title);
    void remove(QString Id);
    void update(QString Id, QString url, QString title);
    OPDSFeedBookmark at(int rowNum);


protected:
    void load();
    void save();

    static QSettings* getCfg();
    static QSettings* cfg;

private:
    QList<OPDSFeedBookmark> bookmarksList;
};

#endif // OPDSLIST_H
