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
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Browser");

    ui->btnOpen->setToolTip(tr("Open video folder"));
    ui->btnSnapshotDir->setToolTip(tr("Select snapshot folder"));
    ui->btnMoveImage->setToolTip(tr("Move image from snapshot folder to video folder"));
    ui->btnRefresh->setToolTip(tr("Refresh list"));

    m_lstVideoFormat = g_pSetting->value("VideoFormat").toString().split('|');
    m_lstImageFormat = g_pSetting->value("ImageFormat").toString().split('|');
    m_strSnapshotDir = g_pSetting->value("SnapshotDir").toString();
    ui->lblSnapshot->setText(m_strSnapshotDir);
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
            if(m_lstVideoFormat.contains(mfi.suffix(), Qt::CaseInsensitive))
            {
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
    foreach (Item *item, m_lstCurrentItems)
    {
        delete item;
        qDebug() << "delete";
    }
    m_lstCurrentItems.clear();
    g_pSetting->setValue("LastDir", m_strCurrentDir);
    chakan(m_strCurrentDir);

    QProgressDialog dlg(tr("Loading..."), tr("Abort"), 0, m_mapAllVideoPath.count(), this);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.show();

    // check repeat
    QString strMsg;
    for(QMultiMap<long long, QString>::const_iterator iter1 = m_mapAllVideoPath.constBegin(); iter1 != m_mapAllVideoPath.constEnd(); iter1 ++)
    {
        for(QMultiMap<long long, QString>::const_iterator iter2 = iter1 + 1; iter2 != m_mapAllVideoPath.constEnd(); iter2 ++)
        {
            //if(iter1.key() == iter2.key())
            if(QFileInfo(iter1.value()).dir().absolutePath() == QFileInfo(iter2.value()).dir().absolutePath())
            {
                strMsg = tr("%1 \ncontains multiple files.").arg(QFileInfo(iter1.value()).dir().absolutePath());
            }
        }
    }
    if(!strMsg.isEmpty())
        QMessageBox::warning(this, "", strMsg);


    int index = 0;
    for(QMultiMap<long long, QString>::const_iterator iter = m_mapAllVideoPath.constEnd() - 1; iter != m_mapAllVideoPath.constBegin() - 1; iter --)
    {
        dlg.setValue(index ++);
        if (dlg.wasCanceled())
            break;
        Item *pItem = new Item(this);
        connect(pItem, SIGNAL(sigCurrentVideoPath(QString)), this, SLOT(onCurrentPlayVideoChanged(QString)));
        pItem->setIndex(index);
        pItem->setSize(iter.key());
        pItem->setVideoPath(iter.value());
        pItem->showImage();
        ui->scrollAreaWidgetContents->layout()->addWidget(pItem);
        m_lstCurrentItems << pItem;
        statusBar()->showMessage(tr("Total : %1").arg(index));
        qApp->processEvents();
    }
    dlg.setValue(m_mapAllVideoPath.count());
}

void MainWindow::on_btnOpen_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this, "Select Video Folder", g_pSetting->value("LastDir").toString()).trimmed();
    if(strDir.isEmpty())
        return;
    m_strCurrentDir = strDir;
    setWindowTitle(m_strCurrentDir);
    updateData();
}

void MainWindow::onCurrentPlayVideoChanged(const QString &path)
{
    m_strCurrentPlayVideoPath = path;
    statusBar()->showMessage(m_strCurrentPlayVideoPath);
}

void MainWindow::on_btnSnapshotDir_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this, "Select Snapshot Folder", m_strSnapshotDir).trimmed();
    if(strDir.isEmpty())
        return;
    m_strSnapshotDir = strDir;
    g_pSetting->setValue("SnapshotDir", m_strSnapshotDir);
    ui->lblSnapshot->setText(m_strSnapshotDir);
}

void MainWindow::on_btnMoveImage_clicked()
{
    if(!QFileInfo(m_strSnapshotDir).isDir() || !QFileInfo(m_strCurrentPlayVideoPath).isFile())
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
        bool bCpy = imageFile.copy(QFileInfo(m_strCurrentPlayVideoPath).dir().absolutePath() + "/" + info.fileName());
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
