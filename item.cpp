#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

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

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onPlayVideo()));
}

Item::~Item()
{
    delete ui;
}

void Item::setVideoPath(const QString &path)
{
    m_strVideoPath = path;
    QFileInfo mfi(path);
    ui->pushButton->setText(QString("%1.(%2MB)%3").arg(m_nIndex).arg(mfi.size() / 1024 / 1024).arg(mfi.fileName()));

    showImage(path);
}

void Item::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();
    QMenu menu(this);
    QAction actPlayVideo(tr("Play video"), &menu);
    QAction actOpenDir(tr("Open Dir"), &menu);
    connect(&actPlayVideo, SIGNAL(triggered()), this, SLOT(onPlayVideo()));
    connect(&actOpenDir, SIGNAL(triggered()), this, SLOT(onOpenDir()));

    menu.addAction(&actPlayVideo);
    menu.addAction(&actOpenDir);
    menu.move(cursor().pos());
    menu.exec();
}

void Item::showImage(const QString &path)
{
    QFileInfoList lstImageInfo = QFileInfo(path).dir().entryInfoList();
    int index = 0;
    for(int i = lstImageInfo.count() - 1; i > 0; i --)
    {
        QFileInfo info = lstImageInfo.at(i);
        if(m_lstImageFormat.contains(info.suffix()))
        {
            ItemImageLabel *pLabel = new ItemImageLabel(this);
            ui->imageLayout->insertWidget(0, pLabel);
            pLabel->setImagePath(info.absoluteFilePath());
            //pLabel->setPixmap(QPixmap(info.absoluteFilePath()).scaled(this->size(), Qt::KeepAspectRatio));
            index ++;
        }
    }
}

void Item::onPlayVideo()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_strVideoPath));
}

void Item::onOpenDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_strVideoPath).dir().absolutePath()));
}
