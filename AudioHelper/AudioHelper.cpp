/**
 * @file AudioHelper.cpp
 * @author Asteri5m
 * @date 2025-02-08 0:11:12
 * @brief 音频助手，主要功能为根据预设切换音频设备
 */

#include <QMetaEnum>
#include "AudioHelper.h"
#include "AudioDatabase.h"
#include "TrayManager.h"

AudioHelper::AudioHelper(QObject *parent)
    : ToolModel{ parent }
    , mDatabase(new AudioDatabase(this))
    , mRelatedList(new RelatedList)
    , mServer(new AudioHelperServer(mRelatedList))
    , mConfig(new Config)
    , mModeMap(new QMap<QString, AudioHelperServer::Mode>)
    , mSceneMap(new QMap<QString, AudioHelperServer::Scene>)
{
    mModeMap->insert("进程模式", AudioHelperServer::Process);
    mModeMap->insert("窗口模式", AudioHelperServer::Windows);
    mModeMap->insert("智能模式", AudioHelperServer::Smart);

    mSceneMap->insert("游戏场景", AudioHelperServer::Entertainment);
    mSceneMap->insert("影音场景", AudioHelperServer::Audiovisual);
    mSceneMap->insert("普通场景", AudioHelperServer::Normal);

    // 默认配置
    mConfig->insert("切换时通知", "true");
    mConfig->insert("过滤系统项", "true");
    mConfig->insert("过滤重复项", "true");
    mConfig->insert("任务模式", "窗口模式");
    mConfig->insert("场景识别", "普通场景");

    // 从数据库读取并更新配置
    for (auto it = mConfig->begin(); it != mConfig->end(); ++it)
    {
        (*mConfig)[it.key()] = mDatabase->queryConfig(it.key(),it.value());
    }

    // 从数据库读取关联任务数据
    mDatabase->queryItems("", "", mRelatedList);
    qDebug() << "Loading related data:" << mRelatedList->length();

    mServer->setNotify(mConfig->value("切换时通知") == "true");
    mServer->setMode(mModeMap->value(mConfig->value("任务模式")));
    mServer->setScene(mSceneMap->value(mConfig->value("场景识别")));
    mServer->start();
}

AudioHelper::~AudioHelper()
{
    delete mServer;
    delete mDatabase;
    delete mRelatedList;
    delete mConfig;
    delete mModeMap;
    delete mSceneMap;
}

void AudioHelper::showWindow()
{
    if (mToolWidget == nullptr)
    {
        mToolWidget = new AudioHelperWidget(mRelatedList, mConfig, mDatabase);
        connect(mToolWidget, SIGNAL(closed()), this, SLOT(toolWindowClosed()));
        connect(mToolWidget, SIGNAL(windowEvent(QString,QString)), this, SLOT(toolWindowEvent(QString,QString)));
        connect(mToolWidget, SIGNAL(configChanged(QString,QString)), this, SLOT(saveConfig(QString,QString)));
    }
    mToolWidget->show();
    mToolWidget->activateWindow();
}

void AudioHelper::saveConfig(const QString &key, const QString &value)
{
    if (mDatabase->saveConfig(key, value))
    {
        (*mConfig)[key] = value;
        if (key == "任务模式")
            mServer->setMode(mModeMap->value(value));
        else if (key == "场景识别")
            mServer->setScene(mSceneMap->value(value));
        else if (key == "切换时通知")
            mServer->setNotify(value == "true" ? true : false);

        return;
    }
    qWarning() << QString("save config %1(%2) to database failed").arg(key).arg(value);
}

void AudioHelper::hotKeyEvent(const QString &event)
{
    // {"切换模式", "锁定设备", "切换场景"},
    if (event == "切换模式")
        return nextMode();
    else if (event == "切换场景")
        return nextScene();
    else if (event == "锁定设备")
        return lockDevice();
    qWarning() << "未知快捷键:" << event;
}

void AudioHelper::nextMode()
{
    QStringList modeString = {"进程模式", "窗口模式", "智能模式"};
    QMetaEnum metaEnum = QMetaEnum::fromType<AudioHelperServer::Mode>();
    AudioHelperServer::Mode mode = mServer->mode();
    int count = metaEnum.keyCount();
    saveConfig("任务模式", modeString[(mode + 1) % count]);
    TrayManager::instance().showMessage("任务模式", QString("当前模式已切换至:%1").arg(modeString[(mode + 1) % count]));
    qInfo() << "切换模式" << modeString[(mode + 1) % count];
}

void AudioHelper::nextScene()
{
    QStringList sceneString = {"普通场景", "游戏场景", "影音场景"};
    QMetaEnum metaEnum = QMetaEnum::fromType<AudioHelperServer::Scene>();
    AudioHelperServer::Scene scene = mServer->scene();
    int count = metaEnum.keyCount();
    saveConfig("场景识别", sceneString[(scene + 1) % count]);
    TrayManager::instance().showMessage("场景识别", QString("当前场景已切换至:%1").arg(sceneString[(scene + 1) % count]));
    qInfo() << "切换场景" << sceneString[(scene + 1) % count];
}

void AudioHelper::lockDevice()
{
    bool state = mServer->state();
    if (state)
        mServer->stop();
    else
        mServer->start();
    QString buf =  QString("设备已%1").arg(state ? "锁定" : "解除锁定");
    TrayManager::instance().showMessage("锁定设备", buf);
    qInfo() << buf.toUtf8().constData();
}
