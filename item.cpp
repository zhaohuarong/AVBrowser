#include <QLineEdit>
#include <QInputDialog>
#include <QRect>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "dialogresizepicture.h"
#include "setting.h"
#include "itemimagelabel.h"
#include "define.h"
#include "item.h"
#include "ui_item.h"

Item::Item(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Item),
    m_nIndex(0),
    m_eStatus(Item_No_Image)
{
    ui->setupUi(this);

    //setStyleSheet("background: lightgray;");

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
    QPoint p = e->pos();

    QMap<ItemImageLabel *, QRect> mapAllImageRect;
    foreach(QObject *pObj, this->children())
    {
        ItemImageLabel *pLbl = qobject_cast<ItemImageLabel *>(pObj);
        if(pLbl != nullptr)
        {
            mapAllImageRect.insert(pLbl, QRect(pLbl->pos(), pLbl->size()));
        }
    }

    for(QMap<ItemImageLabel *, QRect>::const_iterator iter = mapAllImageRect.constBegin(); iter != mapAllImageRect.constEnd(); iter ++)
    {
        if(iter.value().contains(p))
        {
            m_strCurrentImageLabel = iter.key()->getImagePath();
            break;
        }
    }

    e->accept();
    QMenu menu(this);
    QAction actPlayVideo(QIcon(":/image/video.png"), tr("Play Video"), &menu);
    QAction actOpenDir(QIcon(":/image/open.png"), tr("Open Dir"), &menu);
    QAction actRename(QIcon(":/image/rename.png"), tr("Rename"), &menu);
    QAction actCutPicture(QIcon(":/image/resize.png"), tr("Cut Picture"), &menu);
    QAction actZoomPicture(QIcon(":/image/scale.png"), tr("Zoom Picture"), &menu);
    QAction actReloadImage(QIcon(":/image/refresh.png"), tr("Reload Image"), &menu);
    QAction actRemoveImage(QIcon(":/image/delete_img.png"), tr("Remove Image"), &menu);
    QAction actMarkStar(QIcon(":/image/love.png"), tr("Mark star"), &menu);
    QAction actRemove(QIcon(":/image/delete.png"), tr("Remove Item"), &menu);
    connect(&actPlayVideo, SIGNAL(triggered()), this, SLOT(onPlayVideo()));
    connect(&actOpenDir, SIGNAL(triggered()), this, SLOT(onOpenDir()));
    connect(&actRename, SIGNAL(triggered()), this, SLOT(onRename()));
    connect(&actCutPicture, SIGNAL(triggered()), this, SLOT(onChangePictureSize()));
    connect(&actZoomPicture, SIGNAL(triggered()), this, SLOT(onZoomPicture()));
    connect(&actReloadImage, SIGNAL(triggered()), this, SLOT(onReloadImage()));
    connect(&actRemoveImage, SIGNAL(triggered()), this, SLOT(onRemoveImage()));
    connect(&actMarkStar, SIGNAL(triggered()), this, SLOT(onMarkStar()));
    connect(&actRemove, SIGNAL(triggered()), this, SLOT(onRemoveItem()));

    menu.addAction(&actPlayVideo);
    menu.addAction(&actOpenDir);
    menu.addAction(&actRename);
    menu.addAction(&actCutPicture);
    menu.addAction(&actZoomPicture);
    menu.addAction(&actReloadImage);
    menu.addAction(&actRemoveImage);
    menu.addAction(&actMarkStar);
    menu.addAction(&actRemove);
    menu.move(cursor().pos());
    menu.exec();
}

void Item::showImage()
{
    // set default item height
    setFixedHeight(26);

    ui->pushButton->setText(QString("%1.(%2MB)%3").arg(m_nIndex).arg(m_nSize / 1024 / 1024).arg(QFileInfo(m_strVideoPath).fileName()));

    QFileInfoList lstImageInfo = QFileInfo(m_strVideoPath).dir().entryInfoList();
    int index = 0;
    bool bHaveImage = false;
    for(int i = lstImageInfo.count() - 1; i > 0; i --)
    {
        QFileInfo info = lstImageInfo.at(i);
        if(m_lstImageFormat.contains(info.suffix(), Qt::CaseInsensitive))
        {
            if(!bHaveImage)
            {
                bHaveImage = true;
                // change item height
                setFixedHeight(245);
            }
            ItemImageLabel *pLabel = new ItemImageLabel(this);
            ui->imageLayout->insertWidget(0, pLabel);
            pLabel->setImagePath(info.absoluteFilePath());
            index ++;
            qApp->processEvents();
        }
    }

//    if(!bHaveImage)
//        setFixedHeight(ui->pushButton->height());
//    else
//        setFixedHeight(245);
    bool bShow = (QFileInfo(m_strVideoPath).fileName().contains("(精)"));
    QString sheet;
    //ui->pushButton->setStyleSheet(QString("text-align: left; background-color: %1; color: %2").arg(bHaveImage ? "rgb(0, 255, 0)" : "rgb(255, 0, 0)").arg(bShow ? "rgb(255, 215, 0)" : "rgb(0, 0, 0)"));
    if(bHaveImage && bShow)
    {
        m_eStatus = Item_Show;
        sheet = "text-align: left; background-color: rgb(255, 77, 166)";
    }
    else if(bHaveImage && !bShow)
    {
        m_eStatus = Item_Have_Image;
        sheet = "text-align: left; background-color: rgb(0, 255, 0)";
    }
    else
    {
        m_eStatus = Item_No_Image;
        sheet = "text-align: left; background-color: rgb(255, 0, 0)";
    }
    ui->pushButton->setStyleSheet(sheet);
}

void Item::onPlayVideo()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_strVideoPath));
    emit sigCurrentVideoPath(this);
}

void Item::onOpenDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_strVideoPath).dir().absolutePath()));
}

void Item::onRename()
{
    bool ok = false;
    QString name = QFileInfo(m_strVideoPath).fileName();
    QString strNewName = QInputDialog::getText(nullptr, tr("Rename"), tr("Input new name"), QLineEdit::Normal, name, &ok);
    if(ok && !strNewName.trimmed().isEmpty())
    {
        QString dir = QFileInfo(m_strVideoPath).absoluteDir().absolutePath();
        QString strNewPath = dir + "/" + strNewName;

        bool b = QFile::rename(m_strVideoPath, strNewPath);
        QMessageBox::information(nullptr, tr("rename"), b ? tr("rename success") : tr("rename failure"));
        if(b)
        {
            m_strVideoPath = strNewPath;
            onReloadImage();
        }
    }
}

void Item::onRemoveItem()
{
    emit sigRemoveItem(this);
}

void Item::onZoomPicture()
{
    QStringList lstImagePath;
    QObjectList lstObject = children();
    foreach(QObject *obj, lstObject)
    {
        ItemImageLabel *item = qobject_cast<ItemImageLabel *>(obj);
        if(item != nullptr)
        {
            lstImagePath << item->getImagePath();
        }
    }

    if(lstImagePath.count() <= 0)
    {
        QMessageBox::warning(nullptr, tr("warning"), tr("No Images"));
        return;
    }

    QImage img(lstImagePath.at(0));
    bool ok = false;
    int zoom = QInputDialog::getInt(nullptr, tr("zoom"), tr("%1 * %2\nInput zoom rate").arg(img.width()).arg(img.height()), 100, 10, 100, 1, &ok);
    if(!ok)
        return;
    QSize newSize = (float)zoom / 100 * img.size();
    foreach(QString strPath, lstImagePath)
    {
        QImage orgImg(strPath);
        QImage zoomImg = orgImg.scaled(newSize);
        zoomImg.save(strPath);
    }
    onReloadImage();
}

void Item::onChangePictureSize()
{
    QStringList lstImagePath;
    QObjectList lstObject = children();
    foreach(QObject *obj, lstObject)
    {
        ItemImageLabel *item = qobject_cast<ItemImageLabel *>(obj);
        if(item != nullptr)
        {
            lstImagePath << item->getImagePath();
        }
    }

    if(lstImagePath.count() <= 0)
    {
        QMessageBox::warning(nullptr, tr("warning"), tr("No Images"));
        return;
    }

    QImage img(lstImagePath.at(0));
    DialogResizePicture dlg(nullptr);
    dlg.setImage(img);
    dlg.setWidthAndHeightValue(img.width(), img.height());
    if(dlg.exec() != QDialog::Accepted)
        return;
    //QSize wh = dlg.getWidthAndHeightValue();
    QRect rect = dlg.getRect();
    foreach(QString strPath, lstImagePath)
    {
        QImage orgImg(strPath);
        QImage scaledImg = orgImg.copy(rect);
        scaledImg.save(strPath);
    }
    onReloadImage();
}

void Item::onRemoveImage()
{
    QFileInfo fileInfo(m_strCurrentImageLabel);
    if(fileInfo.isFile() || fileInfo.isSymLink())
    {
        QFile::setPermissions(m_strCurrentImageLabel, QFile::WriteOwner);
        if(QFile::remove(m_strCurrentImageLabel))
        {
            //qDebug() << "remove file" << filePath << " faild!";
            onReloadImage();
        }
    }
}

void Item::onMarkStar()
{
    QString dir = QFileInfo(m_strVideoPath).absoluteDir().absolutePath();
    QString name = QFileInfo(m_strVideoPath).fileName();
    qDebug() << m_strVideoPath;
    qDebug() << name;
    qDebug() << dir;
    QString strNewName = dir + "/" + "(精)" + name;
    bool b = QFile::rename(m_strVideoPath, strNewName);
    QMessageBox::information(nullptr, tr("rename"), b ? tr("rename success") : tr("rename failure"));
    if(b)
    {
        m_strVideoPath = strNewName;
        onReloadImage();
    }
}

void Item::cleanImage()
{
    QObjectList lstObject = children();
    foreach(QObject *obj, lstObject)
    {
        ItemImageLabel *item = qobject_cast<ItemImageLabel *>(obj);
        if(item != nullptr)
        {
            ui->imageLayout->removeWidget(item);
            delete item;
            item = nullptr;
        }
    }
}

void Item::onReloadImage()
{
    cleanImage();
    showImage();
}
