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

    explicit OPDSList(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    void add(const QString& url, const QString& title);
    void remove(const QString& Id);
    void update(const QString& Id, const QString& url, const QString& title);
    OPDSFeedBookmark at(int rowNum) const;


protected:
    void load();
    void save() const;

    static QSettings* getCfg();
    static QSettings* cfg;

private:
    QList<OPDSFeedBookmark> bookmarksList;
};

#endif // OPDSLIST_H
