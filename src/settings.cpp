#include "settings.h"

QSettings& Settings::getCfg()
{
    static QSettings instance("YLSoftware", "qOPDSbrowser");
    instance.setFallbacksEnabled(true);
    return instance;
}


QMap<QString, QString> Settings::getUserAgentVariants()
{
    static QMap<QString, QString> variants;
    if (variants.isEmpty())
    {
        variants.insert("Desktop Browser", "Mozilla/6.0");
        variants.insert("Curl", "curl/8.8.0");
        variants.insert("Wget", "Wget/1.21.3");
        variants.insert("CoolReader", "CoolReader/3(Android)");
        variants.insert("FBReader", "Fbreader/2.5.9 (Android 6.0, Samsung, SM-G925)");
        variants.insert("qOPDSbrowser", "qOPDSbrowser/1.0");
    }
    return variants;

}

QString Settings::getUserAgent()
{
    QString userAgentName = getCfg().value("userAgentName", "qOPDSbrowser").toString();
    QMap<QString,QString> userAgentVariant = getUserAgentVariants();
    if (userAgentVariant.keys().contains(userAgentName))
    {
        return userAgentVariant.value(userAgentName);
    }
    return userAgentVariant.value("qOPDSbrowser");
}

QString Settings::getUserAgentName()
{
    return getCfg().value("userAgentName", "qOPDSbrowser").toString();
}


void Settings::setUserAgentName(const QString& userAgentName)
{
    getCfg().setValue("userAgentName", userAgentName);
}

bool Settings::getOpenAfterDownload()
{
    return getCfg().value("openAfterDownload", false).toBool();
}

void Settings::setOpenAfterDownload(bool openAfterDownload)
{
    getCfg().setValue("openAfterDownload", openAfterDownload);
}

QString Settings::getDefaultSaveDirectory()
{
    return getCfg().value("defaultSaveDirectory", "").toString();
}

void Settings::setDefaultSaveDirectory(const QString& dir)
{
    getCfg().setValue("defaultSaveDirectory", dir);
}


QByteArray Settings::getMainWindowState() {
    return getCfg().value("mainWindowState").toByteArray();
}

void Settings::setMainWindowState(const QByteArray& data) {
    getCfg().setValue("mainWindowState", data);
}

bool Settings::haveMainWindowState() {
    return getCfg().contains("mainWindowState");
}

QByteArray Settings::getMainWindowGeometry() {
    return getCfg().value("mainWindowGeometry").toByteArray();
}

void Settings::setMainWindowGeometry(const QByteArray& data) {
    getCfg().setValue("mainWindowGeometry", data);
}

bool Settings::haveMainWindowGeometry() {
    return getCfg().contains("mainWindowGeometry");
}


QByteArray Settings::getBrowserTableViewState() {
    return getCfg().value("browserTableViewState").toByteArray();
}

void Settings::setBrowserTableViewState(const QByteArray& data) {
    getCfg().setValue("browserTableViewState", data);
}

bool Settings::haveBrowserTableViewState() {
    return getCfg().contains("browserTableViewState");
}
