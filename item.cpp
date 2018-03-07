#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "setting.h"
#include "itemimagelabel.h"
#include "define.h"
#include "item.h"
#include "ui_item.h"

Item::Item(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Item),
    m_nIndex(0)
{
    ui->setupUi(this);

    int nHeight = g_pSetting->value("ItemHeight").toInt();
    if(nHeight != 0)
        setFixedHeight(nHeight);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onPlayVideo()));
}

Item::~Item()
{
    delete ui;
}

void Item::setVideoPath(const QString &path)
{
    m_strVideoPath = path;
}

void Item::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();
    QMenu menu(this);
    QAction actPlayVideo(QIcon(":/image/video.png"), tr("Play Video"), &menu);
    QAction actOpenDir(QIcon(":/image/open.png"), tr("Open Dir"), &menu);
    connect(&actPlayVideo, SIGNAL(triggered()), this, SLOT(onPlayVideo()));
    connect(&actOpenDir, SIGNAL(triggered()), this, SLOT(onOpenDir()));

    menu.addAction(&actPlayVideo);
    menu.addAction(&actOpenDir);
    menu.move(cursor().pos());
    menu.exec();
}

void Item::showImage()
{
    ui->pushButton->setText(QString("%1.(%2MB)%3").arg(m_nIndex).arg(m_nSize / 1024 / 1024).arg(QFileInfo(m_strVideoPath).fileName()));

    QFileInfoList lstImageInfo = QFileInfo(m_strVideoPath).dir().entryInfoList();
    int index = 0;
    bool bHaveImage = false;
    for(int i = lstImageInfo.count() - 1; i > 0; i --)
    {
        QFileInfo info = lstImageInfo.at(i);
        if(m_lstImageFormat.contains(info.suffix(), Qt::CaseInsensitive))
        {
            bHaveImage = true;
            ItemImageLabel *pLabel = new ItemImageLabel(this);
            ui->imageLayout->insertWidget(0, pLabel);
            pLabel->setImagePath(info.absoluteFilePath());
            index ++;
        }
    }

    if(!bHaveImage)
        setFixedHeight(ui->pushButton->height());
    ui->pushButton->setStyleSheet(QString("text-align: left; background-color: %1;").arg(bHaveImage ? "rgb(0, 255, 0)" : "rgb(255, 0, 0)"));
}

void Item::onPlayVideo()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_strVideoPath));
}

void Item::onOpenDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_strVideoPath).dir().absolutePath()));
}
