#include "feedparser.h"

#include <QDomNodeList>
#include <QDebug>

/*
 * Based on https://specs.opds.io/opds-1.2
 *
 */

FeedParser::FeedParser()
{
    parserXml = new QDomDocument();
}

void FeedParser::collectEntries()
{
    QDomNodeList docEntries = parserXml->firstChildElement("feed").elementsByTagName("entry");

    for (int i = 0; i < docEntries.count(); i++)
    {
        FeedEntry fEntry;
        fEntry.title = docEntries.at(i).firstChildElement("title").text();

        QDomElement authorItem = docEntries.at(i).firstChildElement("author");
        if (!authorItem.isNull())
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
                fEntry.summary.type = FeedEntry::Summary::text;
                fEntry.summary.value = summaryItem.text();
            }
            else if (summaryType == "html")
            {
                fEntry.summary.type = FeedEntry::Summary::html;
                fEntry.summary.value = summaryItem.text();
            }
        }

        QDomElement contentItem = docEntries.at(i).firstChildElement("content");
        if (!contentItem.isNull())
        {
            QString contentType = contentItem.attribute("type", "text").toLower();
            if (contentType == "text")
            {
                fEntry.content.type = FeedEntry::Content::text;
                fEntry.content.value = contentItem.text();
            }
            else if (contentType == "html")
            {
                fEntry.content.type = FeedEntry::Content::html;
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

            //qDebug() << i << fEntry.title << "Rel:" << linkRel << ", Type:" << linkType << ", Url: " << baseUrl;

            FeedEntryLink fLink;
            fLink.link = baseUrl.resolved(linkItem.attribute("href", "")).toString();
            fLink.type = linkType;

            bool linkToFeed = linkType.split(";").contains("profile=opds-catalog") || linkType.split(";").contains("application/atom+xml");

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
            fEntry.entryType = FeedEntry::book;
            fEntry.links.append(bookLinks);
        }
        else if (feedLinks.count() > 0)
        {
            fEntry.entryType = FeedEntry::feed;
            fEntry.links.append(feedLinks);
        }


        if (fEntry.entryType != FeedEntry::unknown)
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

        bool linkToFeed = linkType.split(";").contains("profile=opds-catalog") || linkType.split(";").contains("application/atom+xml");

        if (!linkToFeed)
        {
            continue;
        }

        if (linkRel == "next" || linkRel == "prev" || linkRel == "previous") {
            FeedEntry fEntry;
            fEntry.entryType = FeedEntry::feed;

            fEntry.title = tr("Prveious page");
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

bool FeedParser::parse(QByteArray data, QUrl baseXmlUrl)
{
    feedData.clear();
    baseUrl = baseXmlUrl;
    searchLink = "";

    QString paserXmlErrorMsg;
    int parserXmlErrorLine;
    int parserXmlErrorColumn;

    if (!parserXml->setContent(
        QString::fromUtf8(data),
        true,
        &paserXmlErrorMsg,
        &parserXmlErrorLine,
        &parserXmlErrorColumn)
    )
    {
        errorString = QString(tr("Error at [%1:%2]: %3")).arg(parserXmlErrorLine)
                .arg(parserXmlErrorColumn)
                .arg(paserXmlErrorMsg);
        return false;
    }

    collectEntries();

    collectLinks();

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

bool FeedParser::haveSearch()
{
    return searchLink != "";
}

QString FeedParser::getSearchLink(QString searchTerms)
{
    QString searchUrl = searchLink;

    return searchUrl.replace(QString("%7BsearchTerms%7D"), QUrl::toPercentEncoding(searchTerms));
}
