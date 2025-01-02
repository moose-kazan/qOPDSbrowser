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
            fEntry.entryType = FeedEntry::book;
            QDomElement authorName = docEntries.at(i).firstChildElement("author").firstChildElement("name");
            if (!authorName.isNull())
            {
                fEntry.title = authorName.text() + " - " + fEntry.title;
            }
        }


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

            if (fEntry.entryType == FeedEntry::book)
            {
                if (!linkToFeed)
                {
                    fEntry.links.append(fLink);
                }
            }
            else
            {
                if (linkToFeed)
                {
                    fEntry.links.clear();
                    fEntry.links.append(fLink);
                    fEntry.entryType = FeedEntry::feed;
                    break;
                }
            }

            linkItem = linkItem.nextSiblingElement("link");
        }


        if (fEntry.entryType != FeedEntry::unknown)
        {
            feedData.entries.append(fEntry);
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

            feedData.entries.append(fEntry);
        }
    }
}

bool FeedParser::parse(QByteArray data, QUrl baseXmlUrl)
{
    feedData.entries.clear();
    baseUrl = baseXmlUrl;

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

FeedData FeedParser::getData()
{
    return feedData;
}
