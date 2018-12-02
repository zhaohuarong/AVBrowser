#ifndef ITEM_H
#define ITEM_H

#include <QFrame>

namespace Ui {
class Item;
}

class Item : public QFrame
{
    Q_OBJECT

public:
    explicit Item(QWidget *parent = 0);
    ~Item();

    void setIndex(int index) {m_nIndex = index;}
    void setVideoPath(const QString &path);
    QString getVideoPath() {return m_strVideoPath;}
    void setSize(long long nSize){m_nSize = nSize;}
    void showImage();

public slots:
    void onReloadImage();

protected:
    void contextMenuEvent(QContextMenuEvent *e);

signals:
    void sigCurrentVideoPath(Item *item);
    void sigRemoveItem(Item *item);

private slots:
    void onPlayVideo();
    void onOpenDir();
    void onRemoveItem();
    void onChangePictureSize();
    void onRemoveImage();

private:
    void cleanImage();

private:
    Ui::Item *ui;

    QString m_strVideoPath;
    int m_nIndex;
    long long m_nSize;
    QString m_strCurrentImageLabel;
};

#endif // ITEM_H
