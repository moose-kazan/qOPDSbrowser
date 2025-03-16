#ifndef DOWNLOADTABLECONTEXTMENU_H
#define DOWNLOADTABLECONTEXTMENU_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QList>
#include <QMenu>
#include <QUrl>

class DownloadTableContextMenu : QObject
{
    Q_OBJECT
public:
    DownloadTableContextMenu(QWidget *parent);
    QMenu *getMenu();
    void setData(bool downloaded, QString fileName, QUrl downloadLink);

private:
    QAction *itemOpenFile;
    QAction *itemCopyLink;
    QMenu *contextMenu;
    bool dataDownloaded;
    QString dataFileName;
    QUrl dataDownloadLink;

private slots:
    void actionOpenFile();
    void actionCopyLink();
};

#endif // DOWNLOADTABLECONTEXTMENU_H
