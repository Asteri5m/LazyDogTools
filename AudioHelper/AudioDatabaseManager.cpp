#include "AudioDatabaseManager.h"

AudioDatabaseManager& AudioDatabaseManager::instance()
{
    static AudioDatabaseManager instance;
    return instance;
}

AudioDatabaseManager::AudioDatabaseManager()
{
    mdb = QSqlDatabase::addDatabase("QSQLITE", "AudioHelper");
    mdb.setDatabaseName("Data/AudioHelper.db");

    if (!mdb.open()) {
        qCritical() << "Could not connect to database.";
        return;
    }

    if (!createTable()) {
        qCritical() << "create table (RelatedItems) failed:" << QSqlQuery{mdb}.lastError().text();
    }

}

AudioDatabaseManager::~AudioDatabaseManager()
{
    mdb.close();
}

bool AudioDatabaseManager::createTable()
{
    QSqlQuery query(mdb);
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS RelatedItems ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "taskName TEXT, "
                               "taskPath TEXT, "
                               "type TEXT, "
                               "tag TEXT, "
                               "deviceName TEXT, "
                               "deviceId TEXT)";
    return query.exec(createTableQuery);
}

bool AudioDatabaseManager::insertItem(RelatedItem &item)
{
    QSqlQuery query(mdb);
    query.prepare("INSERT INTO RelatedItems (taskName, taskPath, type, tag, deviceName, deviceId) "
                  "VALUES (:taskName, :taskPath, :type, :tag, :deviceName, :deviceId)");
    query.bindValue(":taskName", item.taskInfo.name);
    query.bindValue(":taskPath", item.taskInfo.path);
    query.bindValue(":type", item.typeInfo.type);
    query.bindValue(":tag", item.typeInfo.tag);
    query.bindValue(":deviceName", item.audioDeviceInfo.name);
    query.bindValue(":deviceId", item.audioDeviceInfo.id);

    if (!query.exec()) {
        qWarning() << "Failed to insert item: " << query.lastError().text();
        return false;
    }

    // 获取数据库自增ID并将其返回给 RelatedItem
    item.id = query.lastInsertId().toUInt();

    return true;
}

bool AudioDatabaseManager::updateItem(const RelatedItem &item)
{
    QSqlQuery query(mdb);
    query.prepare("UPDATE RelatedItems SET "
                  "taskName = :taskName, "
                  "taskPath = :taskPath, "
                  "type = :type, "
                  "tag = :tag, "
                  "deviceName = :deviceName, "
                  "deviceId = :deviceId "
                  "WHERE id = :id");
    query.bindValue(":taskName", item.taskInfo.name);
    query.bindValue(":taskPath", item.taskInfo.path);
    query.bindValue(":type", item.typeInfo.type);
    query.bindValue(":tag", item.typeInfo.tag);
    query.bindValue(":deviceName", item.audioDeviceInfo.name);
    query.bindValue(":deviceId", item.audioDeviceInfo.id);
    query.bindValue(":id", item.id);

    return query.exec();
}

bool AudioDatabaseManager::deleteItem(int id)
{
    QSqlQuery query(mdb);
    query.prepare("DELETE FROM RelatedItems WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
}

RelatedList AudioDatabaseManager::queryItems(const QString &key, const QString &value)
{
    RelatedList items;
    QSqlQuery query(mdb);

    // 根据是否有查询条件来决定 SQL 语句
    if (key.isEmpty() || value.isEmpty()) {
        query.prepare("SELECT * FROM RelatedItems");
    } else {
        query.prepare("SELECT * FROM RelatedItems WHERE " + key + " = :value");
        query.bindValue(":value", value);
    }

    if (!query.exec()) {
        qWarning() << "Error: " << query.lastError().text();
        return items;
    }

    while (query.next()) {
        RelatedItem item;
        item.taskInfo.name = query.value("taskName").toString();
        item.taskInfo.path = query.value("taskPath").toString();
        item.typeInfo.type = query.value("type").toString();
        item.typeInfo.tag = query.value("tag").toString();
        item.audioDeviceInfo.name = query.value("deviceName").toString();
        item.audioDeviceInfo.id = query.value("deviceId").toString();
        item.id = query.value("id").toUInt();
        items.append(item);
    }
    return items;
}

QString AudioDatabaseManager::lastError()
{
    QSqlQuery query(mdb);
    return query.lastError().text();
}
