#ifndef FEEDPARSER_H
#define FEEDPARSER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QDomDocument>
#include <QList>
#include <QUrl>
#include <QScopedPointer>

struct FeedEntryLink {
    QString link;
    QString type;
};

struct FeedEntry {
    QString title;
    struct Summary {
        QString value;
        enum class Type {unknown, text, html} type = Type::unknown;
    } summary;
    struct Content {
        QString value;
        enum class Type {unknown, text, html} type = Type::unknown;
    } content;
    enum class EntryType {unknown, feed, book} entryType = EntryType::unknown;
    QList<FeedEntryLink> links;
};

class FeedParser: public QObject
{
    Q_OBJECT
public:
    FeedParser();
    ~FeedParser() override;
    bool parse(const QByteArray& data, const QUrl& baseXmlUrl);
    QString errorLine();
    QList<FeedEntry> getData();
    QString getTitle();
    bool haveSearch() const;
    QString getSearchLink(const QString& searchTerms) const;

private:
    QString errorString;
    QScopedPointer<QDomDocument> parserXml;
    QList<FeedEntry> feedData;
    void collectEntries();
    void collectLinks();
    void collectMetadata();
    QUrl baseUrl;
    QString searchLink;
    QString feedTitle;
};

#endif // FEEDPARSER_H
