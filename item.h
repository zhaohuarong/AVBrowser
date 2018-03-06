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

protected:
    void contextMenuEvent(QContextMenuEvent *e);

private:
    void showImage(const QString &path);

private slots:
    void onPlayVideo();
    void onOpenDir();

private:
    Ui::Item *ui;

    QString m_strVideoPath;
    int m_nIndex;
};

#endif // ITEM_H
