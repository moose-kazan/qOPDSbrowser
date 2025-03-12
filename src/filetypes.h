#ifndef FILETYPES_H
#define FILETYPES_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QMimeDatabase>
#include <QObject>

class FileTypes: public QObject
{
    Q_OBJECT
public:
    FileTypes();

    void reset();
    bool add(QString mimeType, QString link);
    QString getDefaultSuffix();
    QStringList getNameFilters();
    QString getLinkByNameFilter(QString nameFilter);
    int count();
private:
    QStringList nameFilters;
    QMap<QString,QString> filterToLinkMap;
    QString defaultSuffix;
    QMimeDatabase mimeDB;
};

#endif // FILETYPES_H
