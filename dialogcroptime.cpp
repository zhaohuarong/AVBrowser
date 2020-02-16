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

    initComboBox();
}

DialogCropTime::~DialogCropTime()
{
    delete ui;
}

void DialogCropTime::setStartEndTime(const QString &startTime, const QString &endTime)
{
    QTime startT = QTime::fromString(startTime, "h:m:s");
//    ui->spStartH->setValue(startT.hour());
//    ui->spStartM->setValue(startT.minute());
//    ui->spStartS->setValue(startT.second());
    for(int i = 0; i < ui->spStartH->count(); i ++) {
        if(ui->spStartH->itemData(i).toInt() == startT.hour()) {
            ui->spStartH->setCurrentIndex(i);
            break;
        }
    }
    for(int i = 0; i < ui->spStartM->count(); i ++) {
        if(ui->spStartM->itemData(i).toInt() == startT.minute()) {
            ui->spStartM->setCurrentIndex(i);
            break;
        }
    }
    for(int i = 0; i < ui->spStartS->count(); i ++) {
        if(ui->spStartS->itemData(i).toInt() == startT.second()) {
            ui->spStartS->setCurrentIndex(i);
            break;
        }
    }

    QTime endT = QTime::fromString(endTime, "h:m:s");
//    ui->spEndH->setValue(endT.hour());
//    ui->spEndM->setValue(endT.minute());
//    ui->spEndS->setValue(endT.second());
    for(int i = 0; i < ui->spEndH->count(); i ++) {
        if(ui->spEndH->itemData(i).toInt() == endT.hour()) {
            ui->spEndH->setCurrentIndex(i);
            break;
        }
    }
    for(int i = 0; i < ui->spEndM->count(); i ++) {
        if(ui->spEndM->itemData(i).toInt() == endT.minute()) {
            ui->spEndM->setCurrentIndex(i);
            break;
        }
    }
    for(int i = 0; i < ui->spEndS->count(); i ++) {
        if(ui->spEndS->itemData(i).toInt() == endT.second()) {
            ui->spEndS->setCurrentIndex(i);
            break;
        }
    }
}

QString DialogCropTime::getStartTime()
{
    QString h = QString::number(ui->spStartH->currentData().toInt());
    QString m = transfer(ui->spStartM->currentData().toInt());
    QString s = transfer(ui->spStartS->currentData().toInt());

    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

QString DialogCropTime::getEndTime()
{
    QString h = QString::number(ui->spEndH->currentData().toInt());
    QString m = transfer(ui->spEndM->currentData().toInt());
    QString s = transfer(ui->spEndS->currentData().toInt());

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

void DialogCropTime::initComboBox()
{
    for(int i = 0; i < 6; i ++) {
        ui->spStartH->addItem(QString::number(i), i);
        ui->spEndH->addItem(QString::number(i), i);
    }
    for(int i = 0; i < 60; i ++) {
        ui->spStartM->addItem(QString::number(i), i);
        ui->spEndM->addItem(QString::number(i), i);
        ui->spStartS->addItem(QString::number(i), i);
        ui->spEndS->addItem(QString::number(i), i);
    }
}
