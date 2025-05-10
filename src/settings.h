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
    static void setUserAgentName(const QString& userAgent);

    static bool getOpenAfterDownload();
    static void setOpenAfterDownload(bool openAfterDownload);

    static QString getDefaultSaveDirectory();
    static void setDefaultSaveDirectory(const QString& dir);

    static QByteArray getMainWindowState();
    static void setMainWindowState(const QByteArray& data);
    static bool haveMainWindowState();

    static QByteArray getMainWindowGeometry();
    static void setMainWindowGeometry(const QByteArray& data);
    static bool haveMainWindowGeometry();

    static QByteArray getBrowserTableViewState();
    static void setBrowserTableViewState(const QByteArray& data);
    static bool haveBrowserTableViewState();

protected:
    static QSettings* getCfg();
    static QSettings* cfg;
    static QMap<QString,QString> *userAgentVariants;
};

#endif // SETTINGS_H
