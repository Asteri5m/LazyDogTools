#ifndef AUDIODATABASEMANAGER_H
#define AUDIODATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QList>
#include <QDebug>
#include "AudioCustom.h"

class AudioDatabaseManager : public QObject
{
    Q_OBJECT
public:
    static AudioDatabaseManager* instance();

    AudioDatabaseManager(const AudioDatabaseManager&) = delete;
    AudioDatabaseManager& operator=(const AudioDatabaseManager&) = delete;

    bool createTable();

    // 关联任务数据
    bool insertItem(RelatedItem &item);
    bool updateItem(const RelatedItem &item);
    bool deleteItem(int id);
    RelatedList queryItems(const QString &key, const QString &value);

    // 应用配置数据
    bool saveConfig(const QString &key, const QString &value);
    QString queryConfig(const QString &key, const QString &defaultValue);


    QString lastError();

private:
    explicit AudioDatabaseManager();
    ~AudioDatabaseManager();

    static AudioDatabaseManager* mInstance;
    QSqlDatabase mdb;
};

#endif // AUDIODATABASEMANAGER_H
