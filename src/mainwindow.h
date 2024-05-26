#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QFileDialog>

#include "dialogbookmarkadd.h"
#include "dialogbookmarkedit.h"
#include "feedparser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void reloadBookmarks();
    void navigateTo(QUrl url);
    void navigateFinish(QNetworkReply *reply);
    void downloadTo(QUrl url, QString fileName);
    void downloadFinish(QNetworkReply *reply, QString fileName);

    Ui::MainWindow *ui;
    DialogBookmarkAdd *dialogBookmarkAdd;
    DialogBookmarkEdit *dialogBookmarkEdit;

    QListView *bookmarksView;
    QListView *browserView;
    QLineEdit *urlEdit;

    QFileDialog *saveDialog;

    QStandardItemModel *bookmarksViewModel;
    QStandardItemModel *browserViewModel;

    QNetworkAccessManager *downloadManager;
    QNetworkAccessManager *navigateManager;

    FeedParser *feedParser;

private slots:
    void actionExit();
    void actionAbout();
    void actionBookmarkAdd();
    void actionBookmarkEdit();
    void actionBookmarkRemove();
    void actionNavigateBookmark(QModelIndex modelIndex);
    void actionBrowserViewClick(QModelIndex modelIndex);

};
#endif // MAINWINDOW_H