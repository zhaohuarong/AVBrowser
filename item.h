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

    void setVideoPath(const QString &path);

private:
    void showImage(const QString &path);

private slots:
    void onOpen();

private:
    Ui::Item *ui;

    QString m_strVideoPath;
};

#endif // ITEM_H
