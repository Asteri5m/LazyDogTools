/**
 * @file Settings.cpp
 * @author Asteri5m
 * @date 2025-02-07 20:23:46
 * @brief 全局设置
 */

#include "Settings.h"
#include "SettingsWidget.h"
#include "LogHandler.h"
#include "Custom.h"
#include "TrayManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>


Settings::Settings(QObject *parent)
    : ToolModel{ parent }
    , mHotkeyManager{new HotkeyManager(this)}
    , mdbDir(QDir("data"))
    , mdbName("Settings.db")
    , mConfig(new Config)
    , mHotkeyMap(new HotkeyMap)
    , mHotkeyIdMap(new HotkeyIdMap)
    , mNetworkManager(new QNetworkAccessManager(this))
{
    initializeDatabase();

    // 安装全局的事件过滤器
    qApp->installNativeEventFilter(mHotkeyManager);

    // 连接热键按下信号
    connect(mHotkeyManager, SIGNAL(hotkeyPressed(int)), this, SLOT(onHotkeyPressed(int)));

    // 默认配置
    mConfig->insert("开机自启动",    "true");
    mConfig->insert("管理员模式启动", "false");
    mConfig->insert("自动更新",      "true");
    mConfig->insert("debug日志",    "false");

    for (auto it = mConfig->begin(); it != mConfig->end(); ++it)
    {
        (*mConfig)[it.key()] = loadConfigFromDB(it.key(),it.value());
    }

    if (parent == nullptr) return;

    // 日志等级
    LogHandler::instance().setLogLevel((*mConfig)["debug日志"] == "true" ? DebugLevel : InfoLevel);
    LogHandler::instance().clearBuffer();

    // 注册开机自启
    if ((*mConfig)["开机自启动"] == "true")
    {
        if (!UAC::setApplicationStartup(true))
        {
            saveConfigToDB("开机自启动", "false");
            (*mConfig)["开机自启动"] = "false";
        }
    }

    // 加载tool的配置：启用状态、热键等等
    ToolManager& toolManager = ToolManager::instance();
    const ToolInfoMap& allToolsInfo = toolManager.getAllTools();
    for (auto it = allToolsInfo.begin(); it != allToolsInfo.end(); ++it)
    {
        // 启用状态
        bool enabled = loadConfigFromDB("enable:" + it->Name, "true") == "true" ? true : false;
        mConfig->insert("enable:" + it->Name, enabled ? "true" : "false");
        if (enabled)
            toolManager.createTool(it.key());
        else
            toolManager.disableTool(it->Name);

        // 热键
        for (const auto& key : it->HotkeyList)
        {
            QString name = QString("hotkey:%1:%2").arg(it->Name).arg(key);
            QString hotkey = loadConfigFromDB(name, "");
            HotkeyInfo hotkeyInfo{hotkey, false, 0};
            mConfig->insert(name, hotkey);
            mHotkeyMap->insert(name, hotkeyInfo);
            if (enabled && !hotkey.isEmpty())
                registerHotkey(name, QKeySequence(hotkey));
        }
    }
}

Settings::~Settings()
{
    delete mHotkeyManager;
    delete mHotkeyIdMap;
    delete mHotkeyMap;
    delete mConfig;
    delete mNetworkManager;
    mdb.close();

    if (mUpdate)
    {
        QDir tempDirObj(UPDATE_DIR);
        QString appPath = QDir::toNativeSeparators(tempDirObj.filePath(APPLICATION_NAME));
        UAC::runArguments("-update", true, appPath);
    }
}

QString Settings::loadConfig(const QString &key) const
{
    return mConfig->value(key, QString());
}

bool Settings::saveConfig(const QString &key, const QString &value) const
{
    if (saveConfigToDB(key, value))
    {
        mConfig->insert(key, value);
        return true;
    }
    return false;
}

bool Settings::registerHotkey(const QString &key, const QKeySequence &keySequence)
{
    for (int id=1; id <= mHotkeyMap->size()+1; id++)
    {   // 找一个未使用的id
        if (mHotkeyIdMap->contains(id))
            continue;

        qInfo() << "注册快捷键:" << key.mid(7) << keySequence.toString();
        qDebug() << "操作id:" << keySequence.toString() << id;
        bool res = mHotkeyManager->registerHotkey(id, keySequence);
        mHotkeyIdMap->insert(id, key);
        (*mHotkeyMap)[key].id = id;
        (*mHotkeyMap)[key].sign = res;
        (*mHotkeyMap)[key].key = QString(keySequence.toString());
        return res;
    }
    return false;
}

void Settings::unregisterHotkey(const QString &key)
{
    if (!mHotkeyMap->contains(key))
        return;

    int id = mHotkeyMap->value(key).id;
    mHotkeyManager->unregisterHotkey(id);
    (*mHotkeyMap)[key].id = 0;
    (*mHotkeyMap)[key].sign = false;
    (*mHotkeyMap)[key].key = "";
    mHotkeyIdMap->remove(id);
}

bool Settings::queryHotkeyState(const QString &key)
{
    if (!mHotkeyMap->contains(key))
        return false;
    return mHotkeyMap->value(key).sign;
}


// 初始化数据库配置
bool Settings::initializeDatabase()
{
    if (!mdbDir.exists()) mdbDir.mkpath(".");

    if (QSqlDatabase::contains(mdbName))
        mdb = QSqlDatabase::database(mdbName);
    else
    {
        mdb = QSqlDatabase::addDatabase("QSQLITE", mdbName);
        mdb.setDatabaseName(mdbDir.filePath(mdbName));
    }

    if (!mdb.open())
    {
        qCritical() << "Failed to open the database:" << mdb.lastError().text();
        return false;
    }

    QSqlQuery query(mdb);
    return query.exec("CREATE TABLE IF NOT EXISTS settings ("
                      "key TEXT PRIMARY KEY,"
                      "value TEXT)");
}


// 保存配置
bool Settings::saveConfigToDB(const QString &key, const QString &value) const
{
    if (!mdb.isOpen()) {
        qWarning() << "Database is not open, save data failed";
        return false;
    }

    qDebug() << "Insert Data, Key:" << key << ", value:" << value;

    QSqlQuery query(mdb);
    query.prepare("INSERT OR REPLACE INTO settings (key, value) VALUES (:key, :value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    if (!query.exec()) {
        qCritical() << "Failed to save setting:" << query.lastError().text();
        return false;
    }

    return true;
}

// 加载配置
QString Settings::loadConfigFromDB(const QString &key, const QString &defaultValue) const
{
    if (!mdb.isOpen()) {
        qWarning() << "Database is not open, load data failed";
        return defaultValue;
    }

    QSqlQuery query(mdb);
    query.prepare("SELECT value FROM settings WHERE key = :key");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qCritical() << "Load setting failed:" << query.lastError().text();
        return defaultValue;
    }

    if (!query.next()) {
        qDebug() << "Load setting failed: select result is null of " + key;
        if (!defaultValue.isNull())
            saveConfigToDB(key, defaultValue);
        return defaultValue;
    }

    return query.value(0).toString();
}

void Settings::showWindow()
{
    if (mToolWidget == nullptr)
    {
        mToolWidget = new SettingsWidget(this);
        connect(mToolWidget, SIGNAL(closed()), this, SLOT(toolWindowClosed()));
        connect(mToolWidget, SIGNAL(windowEvent(const QString&,const QString&)), this, SLOT(toolWindowEvent(const QString&,const QString&)));
        connect(mToolWidget, SIGNAL(toolActiveChanged()), this, SLOT(onToolActiveChanged()));
    }
    mToolWidget->show();
    mToolWidget->activateWindow();
}

void Settings::onHotkeyPressed(int id)
{
    QStringList infos = mHotkeyIdMap->value(id).split(":");
    qDebug() << QString("id: %1, tool: %2, enevt: %3").arg(infos[0]).arg(infos[1]).arg(infos[2]).toUtf8().constData();
    ToolModel *tool = ToolManager::instance().getCreatedTool(infos[1]);
    if (tool != nullptr)
        tool->hotKeyEvent(infos[2]);
}

void Settings::onToolActiveChanged()
{
    emit toolActiveChanged();
}

void Settings::checkForUpdates()
{
    QString apiUrl = mUsingGiteeAPI ? GITEE_API_URL : GITHUB_API_URL;
    QNetworkRequest request(apiUrl);
    
    // 设置 User-Agent（GitHub API 需要）
    request.setHeader(QNetworkRequest::UserAgentHeader, "LazyDogTools");
    
    if (!mUsingGiteeAPI) {
        // GitHub API 可能需要设置 Accept 头
        request.setHeader(QNetworkRequest::ContentTypeHeader, 
                         "application/vnd.github.v3+json");
    }
    
    QNetworkReply *reply = mNetworkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onUpdateReplyed()));
}

void Settings::onUpdateReplyed()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "更新检查失败:" << reply->errorString();
        
        if (!mUsingGiteeAPI) {
            // GitHub API 失败，切换到 Gitee API 重试
            mUsingGiteeAPI = true;
            qInfo() << "切换到 Gitee API 重试";
            checkForUpdates();
            return;
        }
        
        // 两个 API 都失败了
        mUsingGiteeAPI = false;  // 重置为默认使用 GitHub
        TrayManager::instance().showMessage("检查更新", "检查更新失败, 请检查网络然后稍后重试。");
        return;
    }
    
    // 读取响应数据
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull()) 
    {
        if (mUsingGiteeAPI)
        {
            qWarning() << "Gitee API - 更新检查失败: 无效的更新信息格式";
            TrayManager::instance().showMessage("检查更新", "更新检查失败: 无有效的更新信息");
            mUsingGiteeAPI = false;
            return;
        }
        else
        {
            qWarning() << "GitHub API - 更新检查失败: 无效的更新信息格式";
            mUsingGiteeAPI = true;
            checkForUpdates();
            return;
        }
    }
    
    QJsonObject obj = doc.object();
    QString latestVersion;
    QString downloadUrl;
    QString changelog;
    
    if (!mUsingGiteeAPI)
    {
        // 解析 GitHub API 响应
        latestVersion = obj["tag_name"].toString().replace("v", "");
        // 优先获取 assets 中的 zip 文件下载链接
        const QJsonValue assetsValue = obj.value("assets");
        if (assetsValue.isArray()) {
            const QJsonArray assets = assetsValue.toArray();
            for (int i = 0; i < assets.size(); ++i) {
                const QJsonObject asset = assets.at(i).toObject();
                const QString name = asset.value("name").toString();
                if (name.endsWith(".zip")) {
                    downloadUrl = asset.value("browser_download_url").toString();
                    break;
                }
            }
        }
        changelog = obj.value("body").toString();
    } else {
        // 解析 Gitee API 响应
        latestVersion = obj.value("tag_name").toString().replace("v", "");
        
        // 获取 assets 数组
        const QJsonValue assetsValue = obj.value("assets");
        if (assetsValue.isArray()) {
            const QJsonArray assets = assetsValue.toArray();
            for (int i = 0; i < assets.size(); ++i) {
                const QJsonObject asset = assets.at(i).toObject();
                const QString name = asset.value("name").toString();
                const QString url = asset.value("browser_download_url").toString();
                // 检查是否是压缩包
                if (name.endsWith(".zip")) {
                    downloadUrl = url;
                    // 如果找到第一个zip包就使用
                    break;
                }
            }
        }
        
        if (downloadUrl.isEmpty()) 
        {
            qWarning() << "未在 assets 中找到 zip 包";
            return;
        }
        
        changelog = obj.value("body").toString();
    }

    qDebug() << (mUsingGiteeAPI ? "Gitee" : "GitHub") << "更新信息:";
    qDebug() << "版本:" << latestVersion;
    qDebug() << "下载链接:" << downloadUrl;
    
    // 比较版本号
    if (checkVersion(latestVersion))
    {
        qInfo() << "发现新版本:" << latestVersion;
        if (showMessage(mToolWidget == nullptr ? nullptr : mToolWidget, 
            QString("发现新版本-v%1").arg(latestVersion), changelog, MessageType::Info, "立即更新", "稍后更新" ) == QMessageBox::Accepted)
            return downloadUpPack(downloadUrl);
        qInfo() << "更新已取消";
    } 
    else 
    {
        qInfo() << "当前已是最新版本。";
        if (mNotify) TrayManager::instance().showMessage("检查更新", "当前已是最新版本。");
        mNotify = true;
    }
    
    // 重置为默认使用 GitHub
    mUsingGiteeAPI = false;
}

bool Settings::checkVersion(const QString &remoteVersion)
{
    qDebug() << "当前版本:" << CURRENT_VERSION << "远程版本:" << remoteVersion;
    QStringList currentParts = CURRENT_VERSION.split('.');
    QStringList remoteParts = remoteVersion.split('.');
    
    // 确保至少有3个部分
    while (currentParts.size() < 3) currentParts << "0";
    while (remoteParts.size() < 3) remoteParts << "0";
    
    for (int i = 0; i < 3; ++i) 
    {
        int current = currentParts[i].toInt();
        int remote = remoteParts[i].toInt();
        
        if (remote > current) return true;
        if (remote < current) return false;
    }
    
    return false;
}

// 更新程序，将当前目录下所有文件和文件夹copy到父目录
bool Settings::updateApp()
{
    Sleep(1000);
    QDir currentDir(QCoreApplication::applicationDirPath());
    QDir parentDir = currentDir;
    if (!parentDir.cdUp()) {
        qWarning() << "无法访问父目录";
        return false;
    }

    // 确保父目录存在
    if (!parentDir.exists()) {
        qWarning() << "父目录不存在:" << parentDir.absolutePath();
        return false;
    }

    // 获取父目录路径
    QString parentDirPath = parentDir.absolutePath();
    QString currentDirPath = currentDir.absolutePath();

    if (copyDirectory(currentDirPath, parentDirPath))
    {
        qInfo() << "更新成功!";
        QString appPath = QDir::toNativeSeparators(parentDir.absoluteFilePath(APPLICATION_NAME));
        UAC::runArguments("-clear", true, appPath);
        return true;
    }
    else
    {
        qWarning() << "更新失败!";
        return false;
    }
}

bool Settings::copyDirectory(const QString &sourceDirPath, const QString &targetDirPath)
{
    QDir sourceDir(sourceDirPath);
    if (!sourceDir.exists())
        return false;

    QDir targetDir(targetDirPath);
    if (!targetDir.exists() && !QDir().mkpath(targetDirPath))
            return false;

    QFileInfoList fileList = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto it = fileList.constBegin(); it != fileList.constEnd(); ++it) {
        const QFileInfo &fileInfo = *it;
        QString sourcePath = fileInfo.absoluteFilePath();
        QString targetPath = targetDir.filePath(fileInfo.fileName());

        if (fileInfo.isDir()) {
            if (!copyDirectory(sourcePath, targetPath)) {
                qWarning() << "更新目录失败:" << sourcePath;
                return false;
            }
        } else {
            // 如果目标文件存在，先删除它
            if (QFile::exists(targetPath)) {
                QFile targetFile(targetPath);
                if (!targetFile.remove()) {
                    qWarning() << "无法删除目标文件:" << targetPath;
                    return false;
                }
            }

            if (!QFile::copy(sourcePath, targetPath)) {
                qWarning() << "更新文件失败:" << sourcePath;
                return false;
            }
        }
    }
    return true;
}

void Settings::clearUpdate()
{
    QDir tempDirObj(UPDATE_DIR);
    if (tempDirObj.exists()) {
        tempDirObj.removeRecursively();
    }
    
    qInfo() << "更新完成,当前版本:" << CURRENT_VERSION;
}

void Settings::downloadUpPack(const QString &downloadUrl)
{
    qInfo() << "开始下载更新包";
    TrayManager::instance().showMessage("检查更新", "开始下载更新包,在更新就绪后会通知您。");
    QNetworkRequest request(downloadUrl);
    QNetworkReply *reply = mNetworkManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onDownloadFinished()));
}

void Settings::onDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << "下载更新包失败:" << reply->errorString();
        TrayManager::instance().showMessage("检查更新", "下载更新包失败, 请检查网络然后稍后重试。");
        return;
    }

    QString filePath = QCoreApplication::applicationDirPath() + "/update.zip";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "无法打开文件:" << filePath;
        TrayManager::instance().showMessage("检查更新", "无法打开文件:" + filePath);
        return;
    }

    file.write(reply->readAll());
    file.close();

    qInfo() << "更新包下载完成:" << filePath;   
    installUpdate(filePath);
}

bool Settings::inflateData(const QByteArray &compressedData, QByteArray &uncompressedData)
{
    if (compressedData.isEmpty()) {
        return false;
    }

    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];
    
    // 初始化 zlib
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = compressedData.size();
    strm.next_in = (Bytef*)compressedData.data();

    int ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK) {
        return false;
    }

    // 解压数据
    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)out;

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret < 0) {
            inflateEnd(&strm);
            return false;
        }

        uncompressedData.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);
    return true;
}

bool Settings::extractZip(const QString &zipFile, const QString &targetDir)
{
    QFile file(zipFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开更新包文件";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    // 读取 ZIP 文件头
    while (buffer.pos() < buffer.size()) {
        // ZIP 文件头标识
        char signature[4];
        if (buffer.read(signature, 4) != 4) break;

        // 检查是否是文件头标识 (0x04034b50)
        if (signature[0] != 0x50 || signature[1] != 0x4B || 
            signature[2] != 0x03 || signature[3] != 0x04) {
            break;
        }

        // 读取文件头信息
        quint16 version, flags, method, modTime, modDate;
        quint32 crc, compSize, uncompSize;
        quint16 nameLen, extraLen;

        buffer.read((char*)&version, 2);
        buffer.read((char*)&flags, 2);
        buffer.read((char*)&method, 2);
        buffer.read((char*)&modTime, 2);
        buffer.read((char*)&modDate, 2);
        buffer.read((char*)&crc, 4);
        buffer.read((char*)&compSize, 4);
        buffer.read((char*)&uncompSize, 4);
        buffer.read((char*)&nameLen, 2);
        buffer.read((char*)&extraLen, 2);

        // 读取文件名
        QByteArray nameData = buffer.read(nameLen);
        QString fileName = QString::fromLocal8Bit(nameData);
        
        // 跳过额外字段
        buffer.skip(extraLen);

        // 读取压缩数据
        QByteArray compressedData = buffer.read(compSize);
        QByteArray uncompressedData;

        // 解压文件
        if (method == 0) {  // 存储
            uncompressedData = compressedData;
        } else if (method == 8) {  // DEFLATE
            if (!inflateData(compressedData, uncompressedData)) {
                qWarning() << "解压文件失败:" << fileName;
                continue;
            }
        } else {
            qWarning() << "不支持的压缩方法:" << method;
            continue;
        }

        // 创建目标文件
        QString filePath = targetDir + "/" + fileName;
        QFileInfo fileInfo(filePath);
        
        // 创建目录
        if (fileName.endsWith('/')) {
            QDir().mkpath(filePath);
            continue;
        }

        // 确保目标目录存在
        QDir().mkpath(fileInfo.absolutePath());

        // 写入文件
        QFile outFile(filePath);
        if (outFile.open(QIODevice::WriteOnly)) {
            outFile.write(uncompressedData);
            outFile.close();
        } else {
            qWarning() << "无法写入文件:" << filePath;
        }
    }

    return true;
}

void Settings::installUpdate(const QString &zipFilePath)
{
    qInfo() << "准备安装更新";
    QDir tempDirObj(UPDATE_DIR);
    if (tempDirObj.exists()) {
        tempDirObj.removeRecursively();
    }
    QDir().mkpath(UPDATE_DIR);

    // 解压更新包到临时目录
    if (!extractZip(zipFilePath, UPDATE_DIR)) {
        qWarning() << "解压更新包失败";
        return;
    }
    qInfo() << "解压更新包完成";

    // 删除更新包
    QFile::remove(zipFilePath);

    // 向用户确认重启
    if (showMessage(mToolWidget == nullptr ? nullptr : mToolWidget, 
        "更新完成", "更新完成，是否立即重启？", MessageType::Info, "立即重启", "稍后重启" ) == QMessageBox::Accepted)
    {
        // 运行tmp目录下的程序，使用绝对路径
        QString appPath = QDir::toNativeSeparators(tempDirObj.absoluteFilePath(APPLICATION_NAME));
        UAC::runArguments("-update", true, appPath);
        QCoreApplication::quit();
    }
    else
        mUpdate = true;
}
    
