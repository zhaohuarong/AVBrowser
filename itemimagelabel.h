#ifndef ITEMIMAGELABEL_H
#define ITEMIMAGELABEL_H

#include <QLabel>

class ItemImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ItemImageLabel(QWidget *parent = nullptr);

    void setImagePath(const QString &path);

protected:
    void mouseDoubleClickEvent(QMouseEvent *e);

private:
    QString m_strImagePath;
};

#endif // ITEMIMAGELABEL_H
