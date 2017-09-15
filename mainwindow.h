#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidgetItem;

namespace Ui {
class MainWindow;
}

class Item;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void chakan(const QString &path);

private slots:
    void on_btnOpen_clicked();

private:
    Ui::MainWindow *ui;
    QStringList m_lstAllVideoPath;
    QList<Item *> m_lstCurrentItems;
};

#endif // MAINWINDOW_H
