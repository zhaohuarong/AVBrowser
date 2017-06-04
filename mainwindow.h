#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void chakan(const QString &path);

private slots:
    void on_btnOpen_clicked();
    void onOpenVideo(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QStringList m_lstVideoFormat;
    QStringList m_lstImageFormat;
};

#endif // MAINWINDOW_H
