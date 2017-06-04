#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <QSettings>
#include "setting.h"

Setting *Setting::m_pInstance = NULL;

Setting *Setting::instance()
{
    if(m_pInstance == NULL)
        m_pInstance = new Setting();
    return m_pInstance;
}

QVariant Setting::value(const QString &key)
{
    return m_pSettings->value(QString("config/%1").arg(key), "");
}

Setting::Setting(QObject *parent) :
    QObject(parent)
{
    QString strFilePath = qApp->applicationDirPath() + "/config.ini";
    if(!QFileInfo(strFilePath).exists())
        qDebug() << "no config.ini";
    m_pSettings = new QSettings(strFilePath, QSettings::IniFormat);
}
