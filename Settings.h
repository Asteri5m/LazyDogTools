#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * @file Settings.h
 * @author Asteri5m
 * @date 2025-02-07 20:23:46
 * @brief 全局设置、热键监听及事件响应
 */

#include "ToolManager.h"
#include "HotkeyManager.h"
#include <QSqlDatabase>
#include <QDir>
#include <QNetworkAccessManager>
#include <QtZlib/zlib.h>
#include <QBuffer>

struct HotkeyInfo
{
    QString key;    // 热键值
    bool    sign;   // 是否注册成功
    int     id;     // Map ID
};

typedef QMap<QString, HotkeyInfo> HotkeyMap;
typedef QMap<int, QString> HotkeyIdMap;

inline const QString BUILD_DATE = "2025.08.17";  // 构建日期
inline const QString CURRENT_VERSION = "0.0.3.Beta";  // 当前版本号
inline const QString GITHUB_API_URL = "https://api.github.com/repos/Asteri5m/LazyDogTools/releases/latest";
inline const QString GITEE_API_URL = "https://gitee.com/api/v5/repos/Asteri5m/LazyDogTools/releases/latest";
inline const QString UPDATE_DIR = "update";
inline const QString APPLICATION_NAME = "LazyDogTools.exe";

class Settings : public ToolModel
{
    Q_OBJECT
public:
    Settings(QObject *parent=nullptr);
    ~Settings();
    // 数据查询存储接口
    QString loadConfig(const QString& key) const;
    bool saveConfig(const QString& key, const QString& value) const;
    bool registerHotkey(const QString& key, const QKeySequence &keySequence);
    void unregisterHotkey(const QString& key);
    bool queryHotkeyState(const QString& key);

    // 更新相关
    void checkForUpdates();
    bool checkVersion(const QString &remoteVersion);
    static bool updateApp();
    static bool copyDirectory(const QString &sourceDirPath, const QString &targetDirPath);
    static void clearUpdate();

private:
    QDir mdbDir;
    QString mdbName;
    QSqlDatabase mdb;
    HotkeyManager *mHotkeyManager;
    HotkeyMap *mHotkeyMap;
    HotkeyIdMap *mHotkeyIdMap;
    Config *mConfig;

    QNetworkAccessManager *mNetworkManager;
    bool mUsingGiteeAPI = false;  // 标记当前使用的是哪个API
    bool mNotify = false;       // 自动检测更新时不需要通知"已是最新"
    bool mUpdate = false;       // 是否需要更新

    // 更新相关
    void downloadUpPack(const QString& downloadUrl);
    void installUpdate(const QString &zipFilePath);
    bool extractZip(const QString &zipFile, const QString &targetDir);
    bool inflateData(const QByteArray &compressedData, QByteArray &uncompressedData);

    // 数据库相关操作
    bool initializeDatabase();
    QString loadConfigFromDB(const QString& key, const QString& defaultValue = QString()) const;
    bool saveConfigToDB(const QString& key, const QString& value) const;

signals:
    void toolActiveChanged();
    void updateCheckFailed(const QString &error);
    void updateAvailable(const QString &newVersion, const QString &downloadUrl, const QString &changelog);
    void noUpdateAvailable();

public slots:
    void showWindow();

private slots:
    void onHotkeyPressed(int);
    void onToolActiveChanged();
    void onUpdateReplyed();
    void onDownloadFinished();
};

#endif // SETTINGS_H
