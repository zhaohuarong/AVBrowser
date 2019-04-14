#include <QDebug>
#include <QTime>

#include "dialogcroptime.h"
#include "ui_dialogcroptime.h"

DialogCropTime::DialogCropTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCropTime)
{
    ui->setupUi(this);

    connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    connect(ui->spStartH, SIGNAL(valueChanged(int)), this, SLOT(onStartTimeChanged(int)));
    connect(ui->spStartM, SIGNAL(valueChanged(int)), this, SLOT(onStartTimeChanged(int)));
    connect(ui->spStartS, SIGNAL(valueChanged(int)), this, SLOT(onStartTimeChanged(int)));

    connect(ui->spEndH, SIGNAL(valueChanged(int)), this, SLOT(onEndTimeChanged(int)));
    connect(ui->spEndM, SIGNAL(valueChanged(int)), this, SLOT(onEndTimeChanged(int)));
    connect(ui->spEndS, SIGNAL(valueChanged(int)), this, SLOT(onEndTimeChanged(int)));
}

DialogCropTime::~DialogCropTime()
{
    delete ui;
}

QString DialogCropTime::getStartTime()
{
    QString h = QString::number(ui->spStartH->value());
    QString m = transfer(ui->spStartM->value());
    QString s = transfer(ui->spStartS->value());

    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

QString DialogCropTime::getEndTime()
{
    QString h = QString::number(ui->spEndH->value());
    QString m = transfer(ui->spEndM->value());
    QString s = transfer(ui->spEndS->value());

    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

QString DialogCropTime::transfer(int n) const
{
    QString str;
    if(n < 9)
        str = "0";
    str += QString::number(n);
    return str;
}

void DialogCropTime::onStartTimeChanged(int value)
{
    QSpinBox *p = qobject_cast<QSpinBox *>(sender());
    if(p == nullptr)
        return;
    if(p->objectName().contains("StartH"))
    {
    }
    else if(p->objectName().contains("StartM"))
    {
    }
    else if(p->objectName().contains("StartS"))
    {
    }
}

void DialogCropTime::onEndTimeChanged(int value)
{
    QSpinBox *p = qobject_cast<QSpinBox *>(sender());
    if(p == nullptr)
        return;
    if(p->objectName().contains("EndH"))
    {
    }
    else if(p->objectName().contains("EndM"))
    {
    }
    else if(p->objectName().contains("EndS"))
    {
    }
}
