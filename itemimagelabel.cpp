#include <QDesktopServices>
#include <QMouseEvent>
#include <QDebug>
#include "itemimagelabel.h"

ItemImageLabel::ItemImageLabel(QWidget *parent) :
    QLabel(parent)
{
}

void ItemImageLabel::setImagePath(const QString &path)
{
    QWidget *parentItem = qobject_cast<QWidget *>(parent());
    if(parentItem == NULL)
        return;
    m_strImagePath = path;
    setPixmap(QPixmap(m_strImagePath).scaled(parentItem->size(), Qt::KeepAspectRatio));
}

void ItemImageLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    e->accept();
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_strImagePath));
}
