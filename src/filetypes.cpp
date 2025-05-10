#include "filetypes.h"

FileTypes::FileTypes()
= default;

void FileTypes::reset()
{
    nameFilters.clear();
    filterToLinkMap.clear();
    defaultSuffix = "";
}

bool FileTypes::add(const QString& mimeType, const QString& link)
{
    QString typeSuffix = mimeDB.mimeTypeForName(mimeType).preferredSuffix();
    QString typeComment = mimeDB.mimeTypeForName(mimeType).comment();

    if (typeSuffix == "" || typeSuffix.toLower() == "zip")
    {
        // fb2.zip, rtf.zip and many other
        if (mimeType.startsWith("application/", Qt::CaseInsensitive) && mimeType.endsWith("+zip", Qt::CaseInsensitive))
        {
            QString intExt = mimeType;
            intExt.replace(mimeType.length()-QString("+zip").length(), QString("+zip").length(), "");
            intExt.replace(0, QString("application/").length(), "");

            typeComment = tr("Zip-compressed %1").arg(intExt.toUpper());
            typeSuffix = intExt.toLower() + ".zip";
        }
        else if (mimeType == "application/djvu")
        {
            typeSuffix = "djvu";
            typeComment = "DJVU Document";
        }
    }

    if (typeSuffix != "" && typeComment != "")
    {
        QString filterLine = QString("%1 (*.%2)").arg(typeComment, typeSuffix);
        filterToLinkMap.insert(filterLine, link);
        nameFilters.append(filterLine);

        if (defaultSuffix == "")
        {
            defaultSuffix = typeSuffix;
        }

        return true;
    }


    return false;
}

QString FileTypes::getDefaultSuffix()
{
    return defaultSuffix;
}

QStringList FileTypes::getNameFilters()
{
    return nameFilters;
}

QString FileTypes::getLinkByNameFilter(const QString& nameFilter)
{
    return filterToLinkMap.value(nameFilter);
}

int FileTypes::count()
{
    return nameFilters.count();
}
