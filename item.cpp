#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include "define.h"
#include "item.h"
#include "ui_item.h"

Item::Item(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Item)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onOpen()));
}

Item::~Item()
{
    delete ui;
}

void Item::setVideoPath(const QString &path)
{
    m_strVideoPath = path;
    QFileInfo mfi(path);
    ui->pushButton->setText(QString("(%1MB)%2").arg(mfi.size() / 1024 / 1024).arg(mfi.fileName()));

    showImage(path);
}

void Item::showImage(const QString &path)
{
    QFileInfoList lstImageInfo = QFileInfo(path).dir().entryInfoList();
    int index = 0;
    foreach(QFileInfo info, lstImageInfo)
    {
        if(m_lstImageFormat.contains(info.suffix()))
        {
            QLabel *pLabel = NULL;
            switch(index)
            {
            case 0:
                pLabel = ui->p1;
                break;
            case 1:
                pLabel = ui->p2;
                break;
            case 2:
                pLabel = ui->p3;
                break;
            case 3:
                pLabel = ui->p4;
                break;
            }

            if(pLabel != NULL)
                pLabel->setPixmap(QPixmap(info.absoluteFilePath()).scaled(this->size(), Qt::KeepAspectRatio));

            index ++;
        }
    }
}

void Item::onOpen()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_strVideoPath));
}
