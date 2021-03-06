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
    void setStartEndTime(const QString &startTime, const QString &endTime);

    QString getStartTime();
    QString getEndTime();

private:
    inline QString transfer(int n) const;
    void initComboBox();

private:
    Ui::DialogCropTime *ui;
};

#endif // DIALOGCROPTIME_H
