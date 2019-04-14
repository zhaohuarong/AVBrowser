#ifndef DIALOGCROPTIME_H
#define DIALOGCROPTIME_H

#include <QDialog>

namespace Ui {
class DialogCropTime;
}

class DialogCropTime : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCropTime(QWidget *parent = nullptr);
    ~DialogCropTime();

    QString getStartTime();
    QString getEndTime();

private:
    inline QString transfer(int n) const;

private slots:
    void onStartTimeChanged(int value);
    void onEndTimeChanged(int value);

private:
    Ui::DialogCropTime *ui;
};

#endif // DIALOGCROPTIME_H
