#ifndef OPDSLIST_H
#define OPDSLIST_H

#include <QString>
#include <QSettings>

struct OPDSFeedBookmark
{
    QString id;
    QString url;
    QString title;
};

class OPDSFeedBookmarks
{
public:
    QList<OPDSFeedBookmark> bookmarks;
};

class OPDSList
{
public:
    OPDSList();
    static void add(QString url, QString title);
    static void remove(QString Id);
    static void update(QString Id, QString url, QString title);
    static OPDSFeedBookmarks list();

protected:
    static QSettings* getCfg();
    static QSettings* cfg;
    static void save(OPDSFeedBookmarks data);
};

#endif // OPDSLIST_H
