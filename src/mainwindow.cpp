#include "dialogbookmarkadd.h"
#include "feedparserviewmodel.h"
#include "opdslist.h"
#include "settings.h"
#include "icons.h"
#include "helpers.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QList>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileDialog>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dialogBookmarkAdd = new DialogBookmarkAdd(this, Qt::Dialog);
    dialogBookmarkEdit = new DialogBookmarkEdit(this, Qt::Dialog);
    dialogSettings= new DialogSettings(this, Qt::Dialog);

    downloadManager = new QNetworkAccessManager();
    navigateManager = new QNetworkAccessManager();
    connect(navigateManager, &QNetworkAccessManager::finished, this, &MainWindow::navigateFinish);
    connect(downloadManager, &QNetworkAccessManager::finished, this, &MainWindow::downloadFinish);

    feedParser = new FeedParser();

    bookmarksView = findChild<QListView *>("bookmarksView");
    browserView = findChild<QTableView *>("browserView");
    urlEdit = findChild<QLineEdit *>("urlEdit");
    searchLineEdit = findChild<QLineEdit *>("searchLine");
    tableDownloads = findChild<QTableView *>("tableDownloads");
    statusBar = findChild<QStatusBar *>("statusBar");

    searchLineEdit->addAction(QIcon::fromTheme("system-search", QIcon(icons::systemSearch)), QLineEdit::TrailingPosition);
    searchLineEdit->setEnabled(false);

    saveDialog = new QFileDialog(this);
    saveDialog->setAcceptMode(QFileDialog::AcceptSave);
    saveDialog->setDirectory(Settings::getDefaultSaveDirectory());

    bookmarksViewModel = new OPDSList(this);
    bookmarksView->setModel(bookmarksViewModel);

    browserViewModel = new FeedParserViewModel(this);
    browserView->setModel(browserViewModel);

    downloadHistory = new DownloadHistory(this);
    tableDownloads->setModel(downloadHistory);

    downloadTableContextMenu = new DownloadTableContextMenu(this);

    stateRestore();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actionExit()
{
    close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    stateSave();
    QMainWindow::closeEvent(event);
}

void MainWindow::actionAbout()
{
    QMessageBox::about(this, "qOPDSbrowser", tr("Simple client for OPDS feeds."));
}

void MainWindow::actionBookmarkAdd() const
{
    if (dialogBookmarkAdd->exec() == QDialog::Accepted)
    {
        bookmarksViewModel->add(dialogBookmarkAdd->bookmarkUrl, dialogBookmarkAdd->bookmarkTitle);
    }
}

void MainWindow::actionBookmarkEdit() const
{
    QModelIndexList indexList = bookmarksView->selectionModel()->selectedIndexes();

    if (indexList.count() != 1)
    {
        return;
    }

    OPDSFeedBookmark bm = bookmarksViewModel->at(indexList.at(0).row());

    dialogBookmarkEdit->bookmarkId = bm.id;
    dialogBookmarkEdit->bookmarkTitle = bm.title;
    dialogBookmarkEdit->bookmarkUrl = bm.url;

    if (dialogBookmarkEdit->exec() == QDialog::Accepted)
    {
        bookmarksViewModel->update(dialogBookmarkEdit->bookmarkId, dialogBookmarkEdit->bookmarkUrl, dialogBookmarkEdit->bookmarkTitle);
    }
}

void MainWindow::actionBookmarkRemove()
{
    QModelIndexList indexList = bookmarksView->selectionModel()->selectedIndexes();

    if (indexList.count() != 1)
    {
        return;
    }

    QString id = bookmarksViewModel->at(indexList.at(0).row()).id;

    if (QMessageBox::question(this, tr("Remove bookmark"), tr("Do you want to remove this bookmark?")) == QMessageBox::Yes)
    {
        bookmarksViewModel->remove(id);
    }
}


void MainWindow::actionBookmarksViewActivated(const QModelIndex& modelIndex) const
{
    QString url = bookmarksViewModel->at(modelIndex.row()).url;
    navigateTo(url);
}

void MainWindow::navigateTo(const QUrl& url) const
{
    //qDebug() << "navigate to:" << url;
    QString statusBarMessage = QString(tr("Loading feed %1")).arg(url.toString());
    statusBar->showMessage(statusBarMessage, STATUSBAR_MESSAGE_TIMEOUT);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", Settings::getUserAgent().toUtf8());

    navigateManager->get(request);
}

void MainWindow::navigateFinish(QNetworkReply *reply)
{
    if (reply->error())
    {
        QString loadErrorMsg = tr("Can't load %1: %2").arg(reply->url().toString(), reply->errorString());

        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid()) {
            loadErrorMsg = tr("Can't load %1 with http code %2: %3").arg(reply->url().toString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString(), reply->errorString());
        }


        QString statusBarMessage = QString(tr("Can't load feed"));
        statusBar->showMessage(statusBarMessage, STATUSBAR_MESSAGE_TIMEOUT);

        QMessageBox::critical(
            this,
            tr("Can't load url"),
            loadErrorMsg
        );

        return;
    }

    QByteArray responseBody = reply->readAll();
    QUrl url = reply->url();

    if (!feedParser->parse(responseBody, url))
    {
        QString statusBarMessage = QString(tr("Broken feed loaded"));
        statusBar->showMessage(statusBarMessage, STATUSBAR_MESSAGE_TIMEOUT);

        QMessageBox::critical(
            this,
            tr("Can't parse url"),
            tr("Can't parse %1: %2").arg(url.toString(), feedParser->errorLine())
        );

        //qDebug() << responseBody;
        return;
    }


    browserViewModel->populate(feedParser->getData());

    browserView->scrollToTop();

    urlEdit->setText(url.toString());

    switch (urlHistoryDirection)
    {
        case historyGoBack:
            if (urlHistoryIndex > 0)
            {
                urlHistoryIndex--;
            }
        break;

        case historyGoNext:
            if (urlHistoryIndex < urlHistoryList.count()-1)
            {
                urlHistoryIndex++;
            }
        break;

        case historyGoRefresh:
            // Do nothing
        break;

        case historyGoDefault:
            if (urlHistoryIndex <= urlHistoryList.count()-1)
            {
                urlHistoryIndex++;
                urlHistoryList.insert(urlHistoryIndex, url);
            }
            else
            {
                urlHistoryList.append(reply->url());
                urlHistoryIndex = urlHistoryList.count()-1;
            }
        break;
    }

    QString statusBarMessage = QString(tr("Feed loaded"));
    statusBar->showMessage(statusBarMessage, STATUSBAR_MESSAGE_TIMEOUT);

    urlHistoryDirection = historyGoDefault;

    searchLineEdit->setEnabled(feedParser->haveSearch());
}

void MainWindow::actionBrowserViewActivated(QModelIndex modelIndex)
{
    FeedEntry feedEntry = browserViewModel->at(modelIndex.row());

    if (feedEntry.entryType == FeedEntry::feed)
    {
        if (feedEntry.links.count() == 0)
        {
            QMessageBox::warning(this, tr("Warning"), tr("No link found for this feed!"));
            return;
        }

        navigateTo(feedEntry.links.at(0).link);
        return;

    }
    else if (feedEntry.entryType == FeedEntry::book) {
        fileTypes.reset();

        for (int i = 0; i < feedEntry.links.count(); i++)
        {
            fileTypes.add(feedEntry.links.at(i).type, feedEntry.links.at(i).link);
        }

        if (fileTypes.count() < 1)
        {
            QMessageBox::warning(this, tr("Warning"), tr("No link found for this book!"));
            return;
        }

        saveDialog->setNameFilters(fileTypes.getNameFilters());
        saveDialog->selectFile(Helpers::cleanFileName(feedEntry.title, fileTypes.getDefaultSuffix()));
        saveDialog->setDefaultSuffix(fileTypes.getDefaultSuffix());
        if (saveDialog->exec() == QDialog::Accepted)
        {
            downloadTo(fileTypes.getLinkByNameFilter(saveDialog->selectedNameFilter()), saveDialog->selectedFiles().at(0));
        }
        return;
    }

    qDebug() << "Not implemented";
    qDebug() << feedEntry.entryType;
}

void MainWindow::downloadTo(const QUrl& url, const QString& fileName)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", Settings::getUserAgent().toUtf8());
    QByteArray downloadID = downloadHistory->HistoryItemAdd(url, fileName);
    request.setRawHeader("Download-ID", downloadID);

    QNetworkReply* networkReply = downloadManager->get(request);
    //connect(downloadManager, &QNetworkAccessManager::finished, this, &MainWindow::downloadFinish);
    connect(networkReply, &QNetworkReply::downloadProgress, [=] (qint64 bytesReceived, qint64 bytesTotal) {
        downloadHistory->HistoryItemProgress(downloadID, bytesReceived, bytesTotal);
    });
}

void MainWindow::downloadFinish(QNetworkReply *reply)
{
    QByteArray downloadID = reply->request().rawHeader("Download-ID");
    DownloadHistoryItem historyItem = downloadHistory->HistoryItemGet(downloadID);

    if (reply->error())
    {
        downloadHistory->HistoryItemFailed(downloadID);
        QMessageBox::critical(
            this,
            tr("Can't load url"),
            tr("Can't load %1: %2").arg(reply->url().toString(), reply->errorString())
        );
        return;
    }

    if (historyItem.isNull)
    {
        QMessageBox::critical(
            this,
            tr("Critical error"),
            tr("Critical error: can't expand download id!")
        );
        return;

    }

    QByteArray responseBody = reply->readAll();

    QFile bookFile(historyItem.fileName);

    if (!bookFile.open(QIODevice::WriteOnly))
    {
        downloadHistory->HistoryItemFailed(downloadID);
        return;
    }

    if (bookFile.write(responseBody) < responseBody.length())
    {
        downloadHistory->HistoryItemFailed(downloadID);
        QMessageBox::critical(
            this,
            tr("Can't save file"),
            tr("Can't save %1: %2").arg(historyItem.fileName).arg(bookFile.error())
        );
        return;
    }

    bookFile.close();
    downloadHistory->HistoryItemSuccess(downloadID);

    if (Settings::getOpenAfterDownload())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(historyItem.fileName));
    }
}

void MainWindow::actionRefresh()
{
    if (urlEdit->text() != "")
    {
        urlHistoryDirection = historyGoRefresh;
        navigateTo(urlEdit->text());
    }
}

void MainWindow::actionGoNext()
{
    if (urlHistoryIndex < urlHistoryList.count()-1)
    {
        urlHistoryDirection = historyGoNext;
        navigateTo(urlHistoryList.at(urlHistoryIndex+1));
    }
}

void MainWindow::actionGoPrev()
{
    if (urlHistoryIndex > 0 && urlHistoryList.count() > 1)
    {
        urlHistoryDirection = historyGoBack;
        navigateTo(urlHistoryList.at(urlHistoryIndex-1));
    }
}

void MainWindow::actionSettings() const
{
    dialogSettings->setUaVariants(Settings::getUserAgentVariants(), Settings::getUserAgentName());
    dialogSettings->setOpenAfterDownload(Settings::getOpenAfterDownload());
    dialogSettings->setDefaultSaveDirectory(Settings::getDefaultSaveDirectory());
    if (dialogSettings->exec() == QDialog::Accepted)
    {
        Settings::setUserAgentName(dialogSettings->getUserAgentName());
        Settings::setOpenAfterDownload(dialogSettings->getOpenAfterDownload());
        Settings::setDefaultSaveDirectory(dialogSettings->getDefaultSaveDirectory());

        saveDialog->setDirectory(dialogSettings->getDefaultSaveDirectory());
    }
}


void MainWindow::actionSearch() const
{
    navigateTo(feedParser->getSearchLink(searchLineEdit->text()));
}

void MainWindow::actionTableDownloadsDoubleClick(const QModelIndex& modelIndex) const
{
    if (downloadHistory->HistoryItemGetByRow(modelIndex.row()).status == DownloadHistoryItem::downloadSuccess)
    {
        QDesktopServices::openUrl(QUrl(downloadHistory->HistoryItemGetByRow(modelIndex.row()).fileName));
    }
}

void MainWindow::actionTableDownloadsCustomContextMenu(const QPoint pos) const
{
    QModelIndex index = tableDownloads->indexAt(pos);
    if (index.row() > -1) {
        tableDownloads->selectRow(index.row());
        const DownloadHistoryItem item = downloadHistory->HistoryItemGetByRow(index.row());
        downloadTableContextMenu->setData(item.status == DownloadHistoryItem::downloadSuccess, item.fileName, item.url);
        downloadTableContextMenu->getMenu()->popup(tableDownloads->viewport()->mapToGlobal(pos));
    }
}
void MainWindow::stateSave() const
{
    Settings::setMainWindowState(saveState());
    Settings::setMainWindowGeometry(saveGeometry());
    Settings::setBrowserTableViewState(browserView->horizontalHeader()->saveState());
}

void MainWindow::stateRestore() {
    if (Settings::haveMainWindowState()) {
        restoreState(Settings::getMainWindowState());
    }

    if (Settings::haveMainWindowGeometry()) {
        restoreGeometry(Settings::getMainWindowGeometry());
    }

    if (Settings::haveBrowserTableViewState()) {
        browserView->horizontalHeader()->restoreState(Settings::getBrowserTableViewState());
    }
}
