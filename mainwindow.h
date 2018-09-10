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
    void updateData();

private slots:
    void on_btnOpen_clicked();
    void on_btnSnapshotDir_clicked();
    void on_btnRefresh_clicked();
    void on_btnMoveImage_clicked();
    void onCurrentPlayVideoChanged(Item *item);
    void onRemoveItem(Item *item);
    void on_chkBest_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QMultiMap<long long, QString> m_mapAllVideoPath;
    QString m_strCurrentDir;
    QList<Item *> m_lstCurrentItems;
    //QString m_strCurrentPlayVideoPath;
    Item *m_pCurrentItem;
    QString m_strSnapshotDir;
    QStringList m_lstAllSuffix; //all file tpye list
};

#endif // MAINWINDOW_H
