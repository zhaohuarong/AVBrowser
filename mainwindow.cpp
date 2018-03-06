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

    m_lstVideoFormat = g_pSetting->value("videoFormat").toString().split('|');
    m_lstImageFormat = g_pSetting->value("imageFormat").toString().split('|');
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
            if(m_lstVideoFormat.contains(mfi.suffix()))
            {
                m_lstAllVideoPath << mfi.absoluteFilePath();
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

void MainWindow::on_btnOpen_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this, "", g_pSetting->value("lastDir").toString());
    if(strDir.trimmed().isEmpty())
        return;
    m_lstAllVideoPath.clear();
    foreach (Item *item, m_lstCurrentItems)
    {
        delete item;
        qDebug() << "delete";
    }
    m_lstCurrentItems.clear();
    g_pSetting->setValue("lastDir", strDir);
    chakan(strDir);

    QProgressDialog dlg(tr("Loading..."), tr("Abort"), 0, m_lstAllVideoPath.count(), this);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.show();

    int index = 0;
    foreach(QString pathVideo, m_lstAllVideoPath)
    {
        dlg.setValue(index ++);
        if (dlg.wasCanceled())
            break;
        Item *pItem = new Item(this);
        pItem->setIndex(index);
        pItem->setVideoPath(pathVideo);
        ui->scrollAreaWidgetContents->layout()->addWidget(pItem);
        m_lstCurrentItems << pItem;
        statusBar()->showMessage(tr("Total : %1").arg(index));
        qApp->processEvents();
    }
    dlg.setValue(m_lstAllVideoPath.count());
}
