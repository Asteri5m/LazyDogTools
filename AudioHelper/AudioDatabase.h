#ifndef AUDIODATABASE_H
#define AUDIODATABASE_H

/**
 * @file AudioHelperWidget.h
 * @author Asteri5m
 * @date 2025-02-08 0:53:17
 * @brief AudioHelper的数据库管理器
 */

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QList>
#include <QDebug>
#include "AudioCustom.h"

class AudioDatabase : public QObject
{
    Q_OBJECT
public:
    explicit AudioDatabase(QObject* parent = nullptr);
    ~AudioDatabase();

    bool createTable();

    // 关联任务数据
    bool insertItem(RelatedItem &item);
    bool updateItem(const RelatedItem &item);
    bool deleteItem(int id);
    void queryItems(const QString &key, const QString &value, RelatedList *relatedList);

    // 应用配置数据
    bool saveConfig(const QString &key, const QString &value);
    QString queryConfig(const QString &key, const QString &defaultValue = QString());


    QString lastError();

private:
    QSqlDatabase mdb;
};

#endif // AUDIODATABASE_H
