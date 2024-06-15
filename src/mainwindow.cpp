#include "dialogbookmarkadd.h"
#include "opdslist.h"
#include "quuid.h"
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
    downloadHistory = new DownloadHistory();

    bookmarksView = findChild<QListView *>("bookmarksView");
    browserView = findChild<QListView *>("browserView");
    urlEdit = findChild<QLineEdit *>("urlEdit");

    saveDialog = new QFileDialog(this);
    saveDialog->setAcceptMode(QFileDialog::AcceptSave);

    bookmarksViewModel = new QStandardItemModel(this);
    bookmarksView->setModel(bookmarksViewModel);

    browserViewModel = new QStandardItemModel(this);
    browserView->setModel(browserViewModel);

    reloadBookmarks();

    browserViewModel->setColumnCount(3);
    browserViewModel->removeRows(0, bookmarksViewModel->rowCount());
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
    QMessageBox::about(this, "qOPDSbrowser", "Simple client for OPDS feeds.");
}

void MainWindow::actionBookmarkAdd()
{
    if (dialogBookmarkAdd->exec() == QDialog::Accepted)
    {
        OPDSList::add(dialogBookmarkAdd->bookmarkUrl, dialogBookmarkAdd->bookmarkTitle);
        reloadBookmarks();
    }
}

void MainWindow::actionBookmarkEdit()
{
    QModelIndexList indexList = bookmarksView->selectionModel()->selectedIndexes();

    if (indexList.count() != 1)
    {
        return;
    }

    dialogBookmarkEdit->bookmarkId = bookmarksViewModel->data(indexList.at(0).siblingAtColumn(2)).toString();
    dialogBookmarkEdit->bookmarkTitle = bookmarksViewModel->data(indexList.at(0).siblingAtColumn(0)).toString();
    dialogBookmarkEdit->bookmarkUrl = bookmarksViewModel->data(indexList.at(0).siblingAtColumn(1)).toString();

    if (dialogBookmarkEdit->exec() == QDialog::Accepted)
    {
        OPDSList::update(dialogBookmarkEdit->bookmarkId, dialogBookmarkEdit->bookmarkUrl, dialogBookmarkEdit->bookmarkTitle);
        reloadBookmarks();
    }
}

void MainWindow::actionBookmarkRemove()
{
    QModelIndexList indexList = bookmarksView->selectionModel()->selectedIndexes();

    if (indexList.count() != 1)
    {
        return;
    }

    QString id = bookmarksViewModel->data(indexList.at(0).siblingAtColumn(2)).toString();
    //qDebug() << id;

    if (QMessageBox::question(this, tr("Confirmation"), tr("Are you sure?")) == QMessageBox::Yes)
    {
        OPDSList::remove(id);
        reloadBookmarks();
    }
}

void MainWindow::reloadBookmarks()
{
    OPDSFeedBookmarks bookmarks = OPDSList::list();

    bookmarksViewModel->setColumnCount(3);
    bookmarksViewModel->removeRows(0, bookmarksViewModel->rowCount());

    for (int i = 0; i < bookmarks.bookmarks.count(); i++)
    {
        bookmarksViewModel->insertRows(i, 1);
        bookmarksViewModel->setData(
            bookmarksViewModel->index(i, 0),
            bookmarks.bookmarks.at(i).title
        );
        bookmarksViewModel->setData(
            bookmarksViewModel->index(i, 1),
            bookmarks.bookmarks.at(i).url
        );
        bookmarksViewModel->setData(
            bookmarksViewModel->index(i,2),
            bookmarks.bookmarks.at(i).id
        );
    }
}


void MainWindow::actionBookmarksViewActivated(QModelIndex modelIndex)
{
    QString url = bookmarksViewModel->data(modelIndex.siblingAtColumn(1)).toString();
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
        QMessageBox::critical(
            this,
            tr("Can't load url"),
            tr("Can't load %1: %2").arg(reply->url().toString()).arg(reply->errorString())
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


    FeedData feedData = feedParser->getData();

    browserViewModel->setColumnCount(3);
    browserViewModel->removeRows(0, browserViewModel->rowCount());

    for (int i = 0; i < feedData.entries.count(); i++)
    {
        browserViewModel->insertRows(i, 1);
        browserViewModel->setData(
            browserViewModel->index(i, 0),
            feedData.entries.at(i).title
        );

        if (feedData.entries.at(i).entryType == FeedEntry::feed)
        {
            browserViewModel->item(i)->setIcon(QIcon::fromTheme("folder-remote"));
        }
        else if (feedData.entries.at    (i).entryType == FeedEntry::book)
        {
            browserViewModel->item(i)->setIcon(QIcon::fromTheme("text-x-generic"));
        }

        browserViewModel->setData(
            browserViewModel->index(i, 1),
            feedData.entries.at(i).entryType
        );
        if (feedData.entries.at(i).links.count() > 0)
        {
            QString linkData;

            for (int j = 0; j < feedData.entries.at(i).links.count(); j++)
            {
                linkData += feedData.entries.at(i).links.at(j).type + "\n";
                linkData += feedData.entries.at(i).links.at(j).link + "\n";
            }

            browserViewModel->setData(
                browserViewModel->index(i, 2),
                linkData
            );
        }
    }


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

        case historyGoRefresh:    qDebug() << urlHistoryDirection;
        qDebug() << urlHistoryIndex;
        qDebug() << urlHistoryList;

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
}

void MainWindow::actionBrowserViewActivated(QModelIndex modelIndex)
{
    QStringList linkData = browserViewModel->data(modelIndex.siblingAtColumn(2)).toString().split("\n");

    if (browserViewModel->data(modelIndex.siblingAtColumn(1)) == FeedEntry::feed) {
        if (linkData.count() < 2)
        {
            QMessageBox::warning(this, tr("Warning"), tr("No link found for this feed!"));
            return;
        }

        navigateTo(linkData.at(1));
        return;
    }
    else if (browserViewModel->data(modelIndex.siblingAtColumn(1)) == FeedEntry::book) {
        if (linkData.count() < 2)
        {
            QMessageBox::warning(this, tr("Warning"), tr("No link found for this book!"));
            return;
        }

        QMimeDatabase mimeDB;
        QStringList nameFilters;
        QMap<QString,QString> filterToLinkMap;
        for (int i = 0; i < linkData.count(); i+=2)
        {
            QString mimeType = linkData.at(i);
            QString typeSuffix = mimeDB.mimeTypeForName(mimeType).preferredSuffix();
            QString typeComment = mimeDB.mimeTypeForName(mimeType).comment();

            if (typeSuffix == "")
            {
                // fb2.zip, rtf.zip and many other
                if (mimeType.startsWith("application/", Qt::CaseInsensitive) && mimeType.endsWith("+zip", Qt::CaseInsensitive))
                {
                    QString intExt = mimeType;
                    intExt.replace(mimeType.length()-QString("+zip").length(), QString("+zip").length(), "");
                    intExt.replace(0, QString("application/").length(), "");

                    typeSuffix = intExt.toLower() + ".zip";
                    typeComment = tr("Zip-compressed %1").arg(typeSuffix.toUpper());
                }

            }

            if (typeSuffix != "" && typeComment != "")
            {
                QString filterLine = QString("%1 (*.%2)").arg(typeComment).arg(typeSuffix);
                filterToLinkMap.insert(filterLine, linkData.at(i+1));
                nameFilters.append(filterLine);
            }
        }

        saveDialog->setNameFilters(nameFilters);
        if (saveDialog->exec() == QDialog::Accepted)
        {
            downloadTo(filterToLinkMap.value(saveDialog->selectedNameFilter()), saveDialog->selectedFiles().at(0));
        }
        return;
    }

    qDebug() << "Not implemented";
    qDebug() << browserViewModel->data(modelIndex.siblingAtColumn(2)).toString();
}

void MainWindow::downloadTo(QUrl url, QString fileName)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", Settings::getUserAgent().toUtf8());
    request.setRawHeader("Download-ID", downloadHistory->Add(url, fileName));

    downloadManager->get(request);
}

void MainWindow::downloadFinish(QNetworkReply *reply)
{
    if (reply->error())
    {
        QMessageBox::critical(
            this,
            tr("Can't load url"),
            tr("Can't load %1: %2").arg(reply->url().toString()).arg(reply->errorString())
        );
        return;
    }

    DownloadHistoryItem historyItem = downloadHistory->Get(reply->request().rawHeader("Download-ID"));

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
        return;
    }

    if (bookFile.write(responseBody) < responseBody.length())
    {
        QMessageBox::critical(
            this,
            tr("Can't save file"),
            tr("Can't save %1: %2").arg(historyItem.fileName).arg(bookFile.error())
        );
        return;
    }

    bookFile.close();

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
    if (dialogSettings->exec() == QDialog::Accepted)
    {
        Settings::setUserAgentName(dialogSettings->getUserAgentName());
        Settings::setOpenAfterDownload(dialogSettings->getOpenAfterDownload());
    }
}


