#ifndef SETTING_H
#define SETTING_H

#include <QObject>

class QSettings;

#define g_pSetting Setting::instance()

class Setting : public QObject
{
    Q_OBJECT

public:
    static Setting *instance();
    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key);

private:
    explicit Setting(QObject *parent = 0);
    static Setting *m_pInstance;
    QSettings *m_pSettings;
};

#endif // SETTING_H
