#ifndef FEEDPARSER_H
#define FEEDPARSER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QDomDocument>
#include <QList>
#include <QUrl>

struct FeedEntryLink {
    QString link;
    QString type;
};

struct FeedEntry {
    QString title;
    struct Summary {
        QString value;
        enum {unknown, text, html} type = unknown;
    } summary;
    struct Content {
        QString value;
        enum {unknown, text, html} type = unknown;
    } content;
    enum {unknown, feed, book} entryType = unknown;
    QList<FeedEntryLink> links;
};

class FeedParser: public QObject
{
    Q_OBJECT
public:
    FeedParser();
    bool parse(QByteArray data, QUrl baseXmlUrl);
    QString errorLine();
    QList<FeedEntry> getData();
    bool haveSearch();
    QString getSearchLink(QString searchTerms);

private:
    QString errorString;
    QDomDocument *parserXml;
    QList<FeedEntry> feedData;
    void collectEntries();
    void collectLinks();
    QUrl baseUrl;
    QString searchLink;
};

#endif // FEEDPARSER_H
