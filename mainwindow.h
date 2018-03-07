#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMultiMap>
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
    void on_btnSnapshotDir_clicked();
    void on_btnMoveImage_clicked();
    void onCurrentPlayVideoChanged(const QString &path);

private:
    Ui::MainWindow *ui;
    QMultiMap<long long, QString> m_mapAllVideoPath;
    QList<Item *> m_lstCurrentItems;
    QString m_strCurrentPlayVideoPath;
    QString m_strSnapshotDir;
};

#endif // MAINWINDOW_H
