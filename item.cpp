#include <QLabel>
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
    for(int i = lstImageInfo.count() - 1; i > 0; i --)
    {
        QFileInfo info = lstImageInfo.at(i);
        if(m_lstImageFormat.contains(info.suffix()))
        {
            QLabel *pLabel = new QLabel(this);
            ui->imageLayout->insertWidget(0, pLabel);

            pLabel->setPixmap(QPixmap(info.absoluteFilePath()).scaled(this->size(), Qt::KeepAspectRatio));

            index ++;
        }
    }
}

void Item::onOpen()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_strVideoPath));
}
