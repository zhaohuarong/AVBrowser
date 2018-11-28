#ifndef DIALOGRESIZEPICTURE_H
#define DIALOGRESIZEPICTURE_H

#include <QDialog>

#include <QImage>

namespace Ui {
class DialogResizePicture;
}

class DialogResizePicture : public QDialog
{
    Q_OBJECT

public:
    explicit DialogResizePicture(QWidget *parent = 0);
    ~DialogResizePicture();

    void setWidthAndHeightValue(int w, int h);
    void setImage(const QImage &img);
    QRect getRect();

private slots:
    void onPriview();

private:
    Ui::DialogResizePicture *ui;
    QImage m_img;
};

#endif // DIALOGRESIZEPICTURE_H
