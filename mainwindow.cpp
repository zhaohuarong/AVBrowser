#include <QDesktopServices>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QListWidgetItem>

#include <setting.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_lstVideoFormat = g_pSetting->value("videoFormat").toString().split('|');
    m_lstImageFormat = g_pSetting->value("imageFormat").toString().split('|');
    qDebug() << m_lstVideoFormat << m_lstImageFormat;

    connect(ui->lstFileName, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(onOpenVideo(QListWidgetItem *)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::chakan(const QString &path)
{
    QDir dir(path);
    foreach(QFileInfo mfi ,dir.entryInfoList())
    {
        if(mfi.isFile())
        {
            //qDebug()<< "File :" << mfi.fileName();
            if(m_lstVideoFormat.contains(mfi.suffix()))
            {
                QListWidgetItem *item = new QListWidgetItem(QIcon(":/image/video.png"), mfi.fileName(), ui->lstFileName);
                item->setData(Qt::UserRole, mfi.absoluteFilePath());
                ui->lstFileName->addItem(item);
            }
        }
        else
        {
            if(mfi.fileName()=="." || mfi.fileName() == "..")
                continue;
            //qDebug() << "Entry Dir" << mfi.absoluteFilePath();
            chakan(mfi.absoluteFilePath());
        }
    }
}

void MainWindow::on_btnOpen_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this, "", "");
    if(strDir.trimmed().isEmpty())
        return;
    ui->lstFileName->clear();
    chakan(strDir);
}

void MainWindow::onOpenVideo(QListWidgetItem *item)
{
    QDesktopServices::openUrl(item->data(Qt::UserRole).toString());
}
