#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMultiMap>
#include <QMainWindow>
#include <QSystemTrayIcon>

class QLabel;
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
    static bool deleteDir(const QString &dirName);

protected:
    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    void chakan(const QString &path);
    void updateData();
    QString getTypeNumber();

private slots:
    void on_btnOpen_clicked();
    void on_btnSnapshotDir_clicked();
    void on_btnRefresh_clicked();
    void on_btnMoveImage_clicked();
    void onCurrentPlayVideoChanged(Item *item);
    void onRemoveItem(Item *item);
    void on_chkBest_toggled(bool checked);
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    QMultiMap<long long, QString> m_mapAllVideoPath;
    QString m_strCurrentDir;
    QList<Item *> m_lstCurrentItems;
    //QString m_strCurrentPlayVideoPath;
    Item *m_pCurrentItem;
    QString m_strSnapshotDir;
    QStringList m_lstAllSuffix; //all file tpye list
    QSystemTrayIcon *m_pSysTrayIcon;
    QMenu *m_pTrayMenu;
    QAction *m_pActionShow;
    QAction *m_pActionQuit;
    QLabel *m_pStatusLabel;
};

#endif // MAINWINDOW_H
