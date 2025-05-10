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
    explicit DownloadTableContextMenu(QWidget *parent);
    QMenu *getMenu() const;
    void setData(bool downloaded, const QString& fileName, const QUrl& downloadLink);

private:
    QAction *itemOpenFile;
    QAction *itemCopyLink;
    QMenu *contextMenu;
    bool dataDownloaded = false;
    QString dataFileName;
    QUrl dataDownloadLink;

private slots:
    void actionOpenFile() const;
    void actionCopyLink() const;
};

#endif // DOWNLOADTABLECONTEXTMENU_H
