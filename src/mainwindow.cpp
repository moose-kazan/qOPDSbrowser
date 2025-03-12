#include "dialogbookmarkadd.h"
#include "feedparserviewmodel.h"
#include "opdslist.h"
#include "settings.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QList>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardItem>
#include <QMimeType>
#include <QMimeDatabase>
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
    browserView = findChild<QListView *>("browserView");
    urlEdit = findChild<QLineEdit *>("urlEdit");
    searchLineEdit = findChild<QLineEdit *>("searchLine");
    tableDownloads = findChild<QTableView *>("tableDownloads");

    searchLineEdit->addAction(QIcon::fromTheme("system-search", QIcon(":/icons/icon32-find.png")), QLineEdit::TrailingPosition);
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actionExit()
{
    close();
}

void MainWindow::actionAbout()
{
    QMessageBox::about(this, "qOPDSbrowser", tr("Simple client for OPDS feeds."));
}

void MainWindow::actionBookmarkAdd()
{
    if (dialogBookmarkAdd->exec() == QDialog::Accepted)
    {
        bookmarksViewModel->add(dialogBookmarkAdd->bookmarkUrl, dialogBookmarkAdd->bookmarkTitle);
    }
}

void MainWindow::actionBookmarkEdit()
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


void MainWindow::actionBookmarksViewActivated(QModelIndex modelIndex)
{
    QString url = bookmarksViewModel->at(modelIndex.row()).url;
    navigateTo(url);
}

void MainWindow::navigateTo(QUrl url)
{
    //qDebug() << "navigate to:" << url;

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", Settings::getUserAgent().toUtf8());

    navigateManager->get(request);
}

void MainWindow::navigateFinish(QNetworkReply *reply)
{
    if (reply->error())
    {
        QString loadErrorMsg = tr("Can't load %1: %2").arg(reply->url().toString())
                .arg(reply->errorString());

        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid()) {
            loadErrorMsg = tr("Can't load %1 with http code %2: %3").arg(reply->url().toString())
                    .arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString())
                    .arg(reply->errorString());
        }


        QMessageBox::critical(
            this,
            tr("Can't load url"),
            loadErrorMsg
        );
        return;
    }

    QByteArray responseBody = reply->readAll();
    if (!feedParser->parse(responseBody, reply->url()))
    {
        QMessageBox::critical(
            this,
            tr("Can't parse url"),
            tr("Can't parse %1: %2").arg(reply->url().toString()).arg(feedParser->errorLine())
        );

        //qDebug() << responseBody;

        return;
    }


    browserViewModel->populate(feedParser->getData());

    browserView->scrollToTop();

    urlEdit->setText(reply->url().toString());

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
                urlHistoryList.insert(urlHistoryIndex, reply->url());
            }
            else
            {
                urlHistoryList.append(reply->url());
                urlHistoryIndex = urlHistoryList.count()-1;
            }
        break;
    }

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
        saveDialog->selectFile(feedEntry.title);
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

void MainWindow::downloadTo(QUrl url, QString fileName)
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
            tr("Can't load %1: %2").arg(reply->url().toString()).arg(reply->errorString())
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
        QDesktopServices::openUrl(QUrl(historyItem.fileName));
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

void MainWindow::actionSettings()
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


void MainWindow::actionSearch()
{
    navigateTo(feedParser->getSearchLink(searchLineEdit->text()));
}

void MainWindow::actionTableDownloadsDoubleClick(QModelIndex modelIndex)
{
    if (downloadHistory->HistoryItemGetByRow(modelIndex.row()).status == DownloadHistoryItem::downloadSuccess)
    {
        QDesktopServices::openUrl(QUrl(downloadHistory->HistoryItemGetByRow(modelIndex.row()).fileName));
    }
}
