#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QFileDialog>
#include <QTableWidget>

#include "dialogbookmarkadd.h"
#include "dialogbookmarkedit.h"
#include "dialogsettings.h"
#include "feedparser.h"
#include "downloadhistory.h"
#include "opdslist.h"

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
    void downloadFinish(QNetworkReply *reply);

    Ui::MainWindow *ui;
    DialogBookmarkAdd *dialogBookmarkAdd;
    DialogBookmarkEdit *dialogBookmarkEdit;
    DialogSettings *dialogSettings;

    QListView *bookmarksView;
    QListView *browserView;
    QLineEdit *urlEdit;
    QLineEdit *searchLineEdit;
    QTableView *tableDownloads;

    QFileDialog *saveDialog;

    OPDSList *bookmarksViewModel;
    QStandardItemModel *browserViewModel;

    QNetworkAccessManager *downloadManager;
    QNetworkAccessManager *navigateManager;

    FeedParser *feedParser;
    DownloadHistory *downloadHistory;

    QList<QUrl> urlHistoryList;
    int urlHistoryIndex = -1;
    enum {historyGoDefault, historyGoBack, historyGoRefresh, historyGoNext} urlHistoryDirection = historyGoDefault;

private slots:
    void actionExit();
    void actionAbout();
    void actionBookmarkAdd();
    void actionBookmarkEdit();
    void actionBookmarkRemove();
    void actionRefresh();
    void actionGoNext();
    void actionGoPrev();
    void actionSettings();
    void actionSearch();

    void actionBookmarksViewActivated(QModelIndex modelIndex);
    void actionBrowserViewActivated(QModelIndex modelIndex);
};
#endif // MAINWINDOW_H
