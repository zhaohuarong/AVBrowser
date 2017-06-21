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
    void showImages(const QStringList &lstImageFilePath);

private slots:
    void on_btnOpen_clicked();
    void onOpenVideo(QListWidgetItem *item);
    void onCurrentFileChanged(QListWidgetItem *item);
    void onMenuListFile(const QPoint &pos);
    void onOpenFolder();

private:
    Ui::MainWindow *ui;
    QStringList m_lstVideoFormat;
    QStringList m_lstImageFormat;
};

#endif // MAINWINDOW_H
