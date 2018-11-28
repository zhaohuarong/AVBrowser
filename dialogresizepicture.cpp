#include "dialogresizepicture.h"
#include "ui_dialogresizepicture.h"

DialogResizePicture::DialogResizePicture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogResizePicture)
{
    ui->setupUi(this);

    connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->btnPriview, SIGNAL(clicked()), this, SLOT(onPriview()));
}

DialogResizePicture::~DialogResizePicture()
{
    delete ui;
}

void DialogResizePicture::setWidthAndHeightValue(int w, int h)
{
    ui->spinW->setValue(w);
    ui->spinH->setValue(h);
}

void DialogResizePicture::setImage(const QImage &img)
{
    m_img = img;
    ui->lblImg->setPixmap(QPixmap::fromImage(m_img));
}

void DialogResizePicture::onPriview()
{
    ui->lblImg->setPixmap(QPixmap::fromImage(m_img.copy(getRect())));
}

QRect DialogResizePicture::getRect()
{
    int w0 = m_img.width();
    int h0 = m_img.height();
    int w1 = ui->spinW->value();
    int h1 = ui->spinH->value();
    return QRect((w0 - w1) / 2, (h0 - h1) / 2, w1, h1);
}
