#ifndef HELPERS_H
#define HELPERS_H

#include <QObject>

class Helpers
{
public:
    Helpers();
    static QString cleanFileName(QString baseName, const QString& extension = "");
};

#endif // HELPERS_H
