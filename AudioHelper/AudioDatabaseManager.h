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
    static AudioDatabaseManager& instance();

    AudioDatabaseManager(const AudioDatabaseManager&) = delete;
    AudioDatabaseManager& operator=(const AudioDatabaseManager&) = delete;

    bool createTable();
    bool insertItem(RelatedItem &item);
    bool updateItem(const RelatedItem &item);
    bool deleteItem(int id);
    RelatedList queryItems(const QString &key, const QString &value);
    QString lastError();

private:
    explicit AudioDatabaseManager();
    ~AudioDatabaseManager();

    QSqlDatabase mdb;
};

#endif // AUDIODATABASEMANAGER_H
