#include <QCloseEvent>
#include <QScrollBar>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QTextCodec>
#include <QDesktopServices>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QListWidgetItem>
#include <QProgressDialog>

#include <setting.h>
#include "define.h"
#include "item.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

QStringList m_lstVideoFormat;
QStringList m_lstImageFormat;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pCurrentItem(NULL),
    m_pSysTrayIcon(NULL),
    m_pTrayMenu(NULL),
    m_pActionShow(NULL),
    m_pActionQuit(NULL)
{
    ui->setupUi(this);

    setWindowTitle("AVBrowser");

    setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint);

    ui->btnOpen->setToolTip(tr("Open video folder"));
    ui->btnSnapshotDir->setToolTip(tr("Select snapshot folder"));
    ui->btnMoveImage->setToolTip(tr("Move image from snapshot folder to video folder"));
    ui->btnRefresh->setToolTip(tr("Refresh list"));

    m_lstVideoFormat = g_pSetting->value("VideoFormat").toString().split('|');
    m_lstImageFormat = g_pSetting->value("ImageFormat").toString().split('|');
    m_strSnapshotDir = g_pSetting->value("SnapshotDir").toString();
    ui->lblSnapshot->setText(m_strSnapshotDir);

    m_strCurrentDir = g_pSetting->value("LastDir").toString();;
    ui->lblOpenDir->setText(m_strCurrentDir);

    qDebug() << "Video Format:" << m_lstVideoFormat;
    qDebug() << "Audio Format:" << m_lstImageFormat;

    // create tray
    m_pTrayMenu = new QMenu(this);
    m_pActionShow = new QAction(QIcon(":/image/show.png"), tr("Show/Hide"));
    m_pActionQuit = new QAction(QIcon(":/image/quit.png"), tr("Quit"));
    connect(m_pActionShow, SIGNAL(triggered()), this, SLOT(show()));
    connect(m_pActionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    m_pTrayMenu->addAction(m_pActionShow);
    m_pTrayMenu->addAction(m_pActionQuit);
    m_pSysTrayIcon = new QSystemTrayIcon(this);
    connect(m_pSysTrayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));
    QIcon icon = QIcon(":/image/icon.png");
    m_pSysTrayIcon->setIcon(icon);
    m_pSysTrayIcon->setContextMenu(m_pTrayMenu);
    m_pSysTrayIcon->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    hide();
    m_pSysTrayIcon->showMessage(tr("Tips"), tr("Minimized to tray"));
    e->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Escape:
        close();
        break;
    }
}

void MainWindow::chakan(const QString &path)
{
    QDir dir(path);
    foreach(QFileInfo mfi ,dir.entryInfoList())
    {
        if(mfi.isFile())
        {
            if(!m_lstAllSuffix.contains(mfi.suffix()))
                m_lstAllSuffix.append(mfi.suffix());
            if(m_lstVideoFormat.contains(mfi.suffix(), Qt::CaseInsensitive))
            {
                if(ui->chkBest->isChecked() && !mfi.fileName().contains("(精)"))
                    continue;
                m_mapAllVideoPath.insert(mfi.size(), mfi.absoluteFilePath());
            }
            qApp->processEvents();
        }
        else
        {
            if(mfi.fileName()=="." || mfi.fileName() == "..")
                continue;
            chakan(mfi.absoluteFilePath());
        }
    }
}

void MainWindow::updateData()
{
    m_mapAllVideoPath.clear();
    m_lstAllSuffix.clear();
    foreach (Item *item, m_lstCurrentItems)
    {
        delete item;
        qDebug() << "delete";
    }
    m_lstCurrentItems.clear();
    g_pSetting->setValue("LastDir", m_strCurrentDir);
    chakan(m_strCurrentDir);

    // check repeat
    QString strMsg, path1, path2;
    for(QMultiMap<long long, QString>::const_iterator iter1 = m_mapAllVideoPath.constBegin(); iter1 != m_mapAllVideoPath.constEnd(); iter1 ++)
    {
        for(QMultiMap<long long, QString>::const_iterator iter2 = iter1 + 1; iter2 != m_mapAllVideoPath.constEnd(); iter2 ++)
        {
            path1 = QFileInfo(iter1.value()).dir().absolutePath();
            path2 = QFileInfo(iter2.value()).dir().absolutePath();
            if(iter1.key() == iter2.key())
            {
                strMsg = tr("%1\n=\n%2\n").arg(iter1.value()).arg(iter2.value());
                break;
            }
            else if(path1 == path2)
            {
                strMsg = tr("%1 \ncontains multiple files.").arg(path1);
                break;
            }
        }
        if(!strMsg.isEmpty())
            break;
    }
    if(!strMsg.isEmpty())
    {
        QMessageBox::warning(this, "", strMsg);
        if(!path1.isEmpty())
            QDesktopServices::openUrl(QUrl::fromLocalFile(path1));
        if(!path2.isEmpty())
            QDesktopServices::openUrl(QUrl::fromLocalFile(path2));
        return;
    }

    QProgressDialog dlg(tr("Loading..."), tr("Abort"), 0, m_mapAllVideoPath.count(), this);
    dlg.resize(500, 10);
    //dlg.setLabelText(QString("%1/%2").arg(0).arg(m_mapAllVideoPath.count()));
    dlg.setWindowModality(Qt::WindowModal);
    dlg.show();

    int index = 0;
    for(QMultiMap<long long, QString>::const_iterator iter = m_mapAllVideoPath.constEnd() - 1; iter != m_mapAllVideoPath.constBegin() - 1; iter --)
    {
        dlg.setValue(index ++);
        if (dlg.wasCanceled())
            break;
        Item *pItem = new Item(this);
        connect(pItem, SIGNAL(sigCurrentVideoPath(Item *)), this, SLOT(onCurrentPlayVideoChanged(Item *)));
        connect(pItem, SIGNAL(sigRemoveItem(Item*)), this, SLOT(onRemoveItem(Item *)));
        pItem->setIndex(index);
        pItem->setSize(iter.key());
        pItem->setVideoPath(iter.value());
        pItem->showImage();
        ui->scrollAreaWidgetContents->layout()->addWidget(pItem);
        m_lstCurrentItems << pItem;
        statusBar()->showMessage(tr("Total : %1/%2(%3)").arg(index).arg(m_mapAllVideoPath.count()).arg(m_lstAllSuffix.join(",")));
        dlg.setLabelText(QString("%1/%2").arg(index).arg(m_mapAllVideoPath.count()));
        qApp->processEvents();

        // scroll to end
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    }
    dlg.setValue(m_mapAllVideoPath.count());
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

void MainWindow::on_btnOpen_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this, tr("Select Video Folder"), g_pSetting->value("LastDir").toString()).trimmed();
    if(strDir.isEmpty())
        return;
    m_strCurrentDir = strDir;
    ui->lblOpenDir->setText(m_strCurrentDir);
    updateData();
}

void MainWindow::onCurrentPlayVideoChanged(Item *item)
{
    m_pCurrentItem = item;
    statusBar()->showMessage(m_pCurrentItem->getVideoPath());
}

void MainWindow::onRemoveItem(Item *item)
{
    if(item == NULL)
        return;

    QMessageBox::StandardButton btn = QMessageBox::question(this, tr("question"), tr("Are you sure delete this folder?"), QMessageBox::Yes | QMessageBox::No);
    if(btn == QMessageBox::No)
        return;

    QFileInfo info(item->getVideoPath());

//    qDebug() << "delete" << item->getVideoPath() << info.absolutePath();
    bool b = deleteDir(info.absolutePath());
    m_pSysTrayIcon->showMessage(tr("tip"), b ? tr("Success") : tr("Failure"));
    item->hide();
//    item->setParent(NULL);
    ui->scrollAreaWidgetContents->layout()->removeWidget(item);
    ui->scrollArea->horizontalScrollBar()->setValue(0);
}

void MainWindow::on_btnSnapshotDir_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this, tr("Select Snapshot Folder"), m_strSnapshotDir).trimmed();
    if(strDir.isEmpty())
        return;
    m_strSnapshotDir = strDir;
    g_pSetting->setValue("SnapshotDir", m_strSnapshotDir);
    ui->lblSnapshot->setText(m_strSnapshotDir);
}

void MainWindow::on_btnMoveImage_clicked()
{
    if(m_pCurrentItem == NULL || m_strSnapshotDir.isEmpty())
        return;
    if(!QFileInfo(m_strSnapshotDir).isDir() || !QFileInfo(m_pCurrentItem->getVideoPath()).isFile())
    {
        QMessageBox::critical(this, "", tr("src or dst is no exists"));
        return;
    }


    int nCount = 0, nCpy = 0, nDel = 0;
    foreach(QFileInfo info, QDir(m_strSnapshotDir).entryInfoList())
    {
        if(info.isDir())
            continue;
        if(!m_lstImageFormat.contains(info.suffix()))
            continue;
        QFile imageFile(info.absoluteFilePath());
        bool bCpy = imageFile.copy(QFileInfo(m_pCurrentItem->getVideoPath()).dir().absolutePath() + "/" + info.fileName());
        bool bDel = imageFile.remove();
        nCount ++;
        if(bCpy)
            nCpy ++;
        if(bDel)
            nDel ++;
    }
    if(nCount == nCpy && nCount == nDel)
        QMessageBox::information(this, "", tr("Move %1 Images").arg(nCount));
    else
        QMessageBox::critical(this, "", tr("Total: %1\nCopy: %2\nDelete: %3").arg(nCount).arg(nCpy).arg(nDel));
    m_pCurrentItem->onReloadImage();
}

void MainWindow::on_btnRefresh_clicked()
{
    if(!QFileInfo(m_strCurrentDir).isDir())
    {
        QMessageBox::critical(this, "", tr("No select dir"));
        return;
    }
    updateData();
}

void MainWindow::on_chkBest_toggled(bool checked)
{
    Q_UNUSED(checked);
    on_btnRefresh_clicked();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        isHidden() ? show() : hide();
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    }
}

bool MainWindow::deleteDir(const QString &dirName)
{
    QDir directory(dirName);
    if (!directory.exists())
    {
        return true;
    }


    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();


    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath))
            {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        }
        else if (fileInfo.isDir())
        {
            if (!deleteDir(filePath))
            {
                error = true;
            }
        }
    }


    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }
    return !error;
}
