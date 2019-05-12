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
#include <QProcess>
#include <QDebug>

#include "dialogcroptime.h"
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

    QMenu menu(this);
    QAction actPlayVideo(QIcon(":/image/video.png"), tr("Play Video"), &menu);
    QAction actOpenDir(QIcon(":/image/open.png"), tr("Open Dir"), &menu);
    QAction actRename(QIcon(":/image/rename.png"), tr("Rename"), &menu);
    QAction actCutVideo(QIcon(":/image/cut.png"), tr("Cut Video"), &menu);
    QAction actCutPicture(QIcon(":/image/resize.png"), tr("Cut Picture"), &menu);
    QAction actZoomPicture(QIcon(":/image/scale.png"), tr("Zoom Picture"), &menu);
    QAction actReloadImage(QIcon(":/image/refresh.png"), tr("Reload Image"), &menu);
    QAction actRemoveImage(QIcon(":/image/delete_img.png"), tr("Remove Image"), &menu);
    QAction actMarkStar(QIcon(":/image/love.png"), tr("Mark star"), &menu);
    QAction actRemove(QIcon(":/image/delete.png"), tr("Remove Item"), &menu);

    connect(&actPlayVideo, SIGNAL(triggered()), this, SLOT(onPlayVideo()));
    connect(&actOpenDir, SIGNAL(triggered()), this, SLOT(onOpenDir()));
    connect(&actRename, SIGNAL(triggered()), this, SLOT(onRename()));
    connect(&actCutVideo, SIGNAL(triggered()), this, SLOT(onCutVideo()));
    connect(&actCutPicture, SIGNAL(triggered()), this, SLOT(onChangePictureSize()));
    connect(&actZoomPicture, SIGNAL(triggered()), this, SLOT(onZoomPicture()));
    connect(&actReloadImage, SIGNAL(triggered()), this, SLOT(onReloadImage()));
    connect(&actRemoveImage, SIGNAL(triggered()), this, SLOT(onRemoveImage()));
    connect(&actMarkStar, SIGNAL(triggered()), this, SLOT(onMarkStar()));
    connect(&actRemove, SIGNAL(triggered()), this, SLOT(onRemoveItem()));

    menu.addAction(&actPlayVideo);
    menu.addAction(&actOpenDir);
    menu.addAction(&actRename);
    menu.addAction(&actCutVideo);
    menu.addAction(&actCutPicture);
    menu.addAction(&actZoomPicture);
    menu.addAction(&actReloadImage);
    menu.addAction(&actRemoveImage);
    menu.addAction(&actMarkStar);
    menu.addAction(&actRemove);
    menu.move(cursor().pos());
    menu.exec();

    e->accept();
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

void Item::onCutVideo()
{
    QString name = QFileInfo(m_strVideoPath).fileName();
    QString strOutName = QFileInfo(m_strVideoPath).absoluteDir().absolutePath() + "/out." + QFileInfo(m_strVideoPath).suffix();

    DialogCropTime dialog;
    dialog.setStartEndTime(m_startTime, m_endTime);
    if(dialog.exec() == QDialog::Accepted)
    {
        QString cmd = QString("ffmpeg -i %1 -vcodec copy -acodec copy -ss %2 -to %3 %4 -y").arg(m_strVideoPath).arg(dialog.getStartTime()).arg(dialog.getEndTime()).arg(strOutName);
        qDebug() << cmd;

        QMessageBox testMassage;
        testMassage.setText(cmd);
        testMassage.show();
        qApp->processEvents();

        QProcess p;
        p.start("cmd", QStringList() << "/c" << cmd);
        p.waitForStarted();
        p.waitForFinished();
        QString strTemp=QString::fromLocal8Bit(p.readAllStandardOutput());
    }
    m_startTime = dialog.getStartTime();
    m_endTime = dialog.getEndTime();
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

    QMap<QString, float> mapZoomRate;
    float delta = 0.125;
    for(int i = 7; i > 0; i --)
    {
        int w = img.width() * delta * i;
        int h = img.height() * delta * i;
        mapZoomRate.insert(QString("%1/8 - %2*%3").arg(i).arg(w).arg(h), delta * i);
    }

    QStringList lstLabel;
    for(QMap<QString, float>::const_iterator iter = mapZoomRate.constBegin(); iter != mapZoomRate.constEnd(); iter ++)
    {
        lstLabel << iter.key();
    }

    QString ss = QInputDialog::getItem(nullptr, tr("zoom"), tr("original size: %1 * %2").arg(img.width()).arg(img.height()), lstLabel, 0, false, &ok);
    if(ok && !ss.isEmpty())
    {
        float zoom = mapZoomRate.value(ss);
        QSize newSize = zoom * img.size();
        foreach(QString strPath, lstImagePath)
        {
            QImage orgImg(strPath);
            QImage zoomImg = orgImg.scaled(newSize);
            zoomImg.save(strPath);
        }
        onReloadImage();
    }
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
    QString strStar = "(精)";
    QString dir = QFileInfo(m_strVideoPath).absoluteDir().absolutePath();
    QString name = QFileInfo(m_strVideoPath).fileName();
    bool bStar = name.contains(strStar);
    QString strNewName;
    if(bStar)
    {
        strNewName = dir + "/" + name.replace(strStar, "");
    }
    else
    {
        strNewName = dir + "/" + strStar + name;
    }

    bool b = QFile::rename(m_strVideoPath, strNewName);
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
