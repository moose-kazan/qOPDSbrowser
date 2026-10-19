#include "feedparser.h"

#include <QDomNodeList>
#include <QDebug>

/*
 * Based on https://specs.opds.io/opds-1.2
 *
 */

FeedParser::FeedParser()
{
    parserXml.reset(new QDomDocument());
}

FeedParser::~FeedParser() = default;

void FeedParser::collectEntries()
{
    QDomNodeList docEntries = parserXml->firstChildElement("feed").elementsByTagName("entry");

    for (int i = 0; i < docEntries.count(); i++)
    {
        FeedEntry fEntry;
        fEntry.title = docEntries.at(i).firstChildElement("title").text();

        if (!docEntries.at(i).firstChildElement("author").isNull())
        {
            QDomElement authorName = docEntries.at(i).firstChildElement("author").firstChildElement("name");
            if (!authorName.isNull() && authorName.text() != "")
            {
                fEntry.title = authorName.text() + " - " + fEntry.title;
            }
        }

        QDomElement summaryItem = docEntries.at(i).firstChildElement("summary");
        if (!summaryItem.isNull())
        {
            QString summaryType = summaryItem.attribute("type", "text").toLower();
            if (summaryType == "text")
            {
                fEntry.summary.type = FeedEntry::Summary::Type::text;
                fEntry.summary.value = summaryItem.text();
            }
            else if (summaryType == "html")
            {
                fEntry.summary.type = FeedEntry::Summary::Type::html;
                fEntry.summary.value = summaryItem.text();
            }
        }

        QDomElement contentItem = docEntries.at(i).firstChildElement("content");
        if (!contentItem.isNull())
        {
            QString contentType = contentItem.attribute("type", "text").toLower();
            if (contentType == "text")
            {
                fEntry.content.type = FeedEntry::Content::Type::text;
                fEntry.content.value = contentItem.text();
            }
            else if (contentType == "html")
            {
                fEntry.content.type = FeedEntry::Content::Type::html;
                fEntry.content.value = contentItem.text();
            }
        }

        QList<FeedEntryLink> bookLinks;
        QList<FeedEntryLink> feedLinks;

        QDomElement linkItem = docEntries.at(i).firstChildElement("link");
        while (!linkItem.isNull()) {
            QString linkRel = linkItem.attribute("rel", "");
            QString linkType = linkItem.attribute("type", "").toLower();

            if (linkRel == "alternate" || linkRel == "related")
            {
                linkItem = linkItem.nextSiblingElement("link");
                continue;
            }

            if (linkType.startsWith("image/"))
            {
                linkItem = linkItem.nextSiblingElement("link");
                continue;
            }

            FeedEntryLink fLink;
            fLink.link = baseUrl.resolved(linkItem.attribute("href", "")).toString();
            fLink.type = linkType;

            const QStringList linkTypeParts = linkType.split(";");
            bool linkToFeed = linkTypeParts.contains("profile=opds-catalog") || linkTypeParts.contains("application/atom+xml");

            if (linkToFeed)
            {
                feedLinks.clear();
                feedLinks.append(fLink);
            }
            else
            {
                bookLinks.append(fLink);
            }

            linkItem = linkItem.nextSiblingElement("link");
        }

        if (bookLinks.count() > 0)
        {
            fEntry.entryType = FeedEntry::EntryType::book;
            fEntry.links.append(bookLinks);
        }
        else if (feedLinks.count() > 0)
        {
            fEntry.entryType = FeedEntry::EntryType::feed;
            fEntry.links.append(feedLinks);
        }


        if (fEntry.entryType != FeedEntry::EntryType::unknown)
        {
            feedData.append(fEntry);
        }
    }
}

void FeedParser::collectLinks()
{
    QDomNodeList docLinks = parserXml->firstChildElement("feed").elementsByTagName("link");

    for (int i = 0; i < docLinks.count(); i++)
    {
        QDomElement linkItem = docLinks.at(i).toElement();

        // Skip broken tags
        if (linkItem.hasChildNodes() || !linkItem.hasAttributes())
        {
            continue;
        }

        QString linkRel = linkItem.attribute("rel", "");
        QString linkType = linkItem.attribute("type", "").toLower();

        const QStringList linkTypeParts = linkType.split(";");
        bool linkToFeed = linkTypeParts.contains("profile=opds-catalog") || linkTypeParts.contains("application/atom+xml");

        if (!linkToFeed)
        {
            continue;
        }

        if (linkRel == "next" || linkRel == "prev" || linkRel == "previous") {
            FeedEntry fEntry;
            fEntry.entryType = FeedEntry::EntryType::feed;

            fEntry.title = tr("Previous page");
            if (linkRel == "next")
            {
                fEntry.title = tr("Next page");
            }

            FeedEntryLink fLink;
            fLink.link = baseUrl.resolved(linkItem.attribute("href", "")).toString();
            fLink.type = linkType;

            fEntry.links.append(fLink);

            feedData.append(fEntry);
        }

        if (linkRel == "search")
        {
            QString linkHref = linkItem.attribute("href", "");
            if (linkHref != "")
            {
                searchLink = baseUrl.resolved(linkHref).toString();
            }
        }
    }
}

void FeedParser::collectMetadata()
{
    QDomElement elementTitle = parserXml->firstChildElement("feed").firstChildElement("title");
    if (!elementTitle.isNull())
    {
        feedTitle = elementTitle.text();
    }
}

bool FeedParser::parse(const QByteArray& data, const QUrl& baseXmlUrl)
{
    feedData.clear();
    baseUrl = baseXmlUrl;
    searchLink = "";
    feedTitle = "";

    QString parserXmlErrorMsg;
    int parserXmlErrorLine;
    int parserXmlErrorColumn;

    QDomDocument::ParseResult parseResult = parserXml->setContent(data);
    if (!parseResult)
    {
        errorString = QString(tr("Error at [%1:%2]: %3")).arg(parseResult.errorLine)
                .arg(parseResult.errorColumn)
                .arg(parseResult.errorMessage);
        return false;
    }

    collectEntries();

    collectLinks();

    collectMetadata();

    errorString = "";
    return true;
}

QString FeedParser::errorLine()
{
    return errorString;
}

QList<FeedEntry> FeedParser::getData()
{
    return feedData;
}

QString FeedParser::getTitle()
{
    return feedTitle;
}

bool FeedParser::haveSearch() const
{
    return searchLink != "";
}

QString FeedParser::getSearchLink(const QString& searchTerms) const
{
    QString searchUrl = searchLink;

    return searchUrl.replace(QString("%7BsearchTerms%7D"), QUrl::toPercentEncoding(searchTerms));
}
