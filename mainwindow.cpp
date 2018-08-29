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
    m_pCurrentItem(NULL)
{
    ui->setupUi(this);

    setWindowTitle("AVBrowser");

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
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
