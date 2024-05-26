#ifndef FEEDPARSER_H
#define FEEDPARSER_H

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
    enum {unknown, feed, book} entryType = unknown;
    QList<FeedEntryLink> links;
};

class FeedData {
public:
    QList<FeedEntry> entries;
};

class FeedParser
{
public:
    FeedParser();
    bool parse(QByteArray data, QUrl baseUrl);
    QString errorLine();
    FeedData getData();

private:
    QString errorString;
    QDomDocument *parserXml;
    FeedData feedData;
};

#endif // FEEDPARSER_H
