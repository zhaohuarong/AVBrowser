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
    void setSize(long long nSize){m_nSize = nSize;}
    void showImage();

protected:
    void contextMenuEvent(QContextMenuEvent *e);

signals:
    void sigCurrentVideoPath(const QString &path);

private slots:
    void onPlayVideo();
    void onOpenDir();
    void onReloadImage();

private:
    void cleanImage();

private:
    Ui::Item *ui;

    QString m_strVideoPath;
    int m_nIndex;
    long long m_nSize;
};

#endif // ITEM_H
