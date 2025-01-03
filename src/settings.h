#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{

public:
    Settings();
    static QMap<QString, QString> getUserAgentVariants();
    static QString getUserAgent();
    static QString getUserAgentName();
    static void setUserAgentName(QString userAgent);
    static bool getOpenAfterDownload();
    static void setOpenAfterDownload(bool openAfterDownload);
    static QString getDefaultSaveDirectory();
    static void setDefaultSaveDirectory(QString dir);

protected:
    static QSettings* getCfg();
    static QSettings* cfg;
    static QMap<QString,QString> *userAgentVariants;
};

#endif // SETTINGS_H
