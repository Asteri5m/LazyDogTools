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
    , mIgnoreMap(new IgnoreMap)
    , mServer(new AudioHelperServer(mRelatedList, mIgnoreMap))
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

    // 校验关联数据：音频设备会发生变化,id会自动变化，设备会插拔
    checkRelateds();

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

void AudioHelper::checkRelateds()
{
    // 获取当前的所有设备信息
    AudioDeviceList deviceList;
    AudioManager::getAudioOutDeviceList(&deviceList);
    QStringList nameList = deviceList.keys();
    QStringList idList = deviceList.values();
    // 建立反向map
    QMap<QString, QString> idToName;
    // 记录询问过并确认要替换的设备:old id - new id
    QMap<QString, QString> changeDevice;
    for (auto it = deviceList.constBegin(); it != deviceList.constEnd(); ++it) {
        idToName[it.value()] = it.key();
    }

    for (auto item = mRelatedList->begin(); item != mRelatedList->end(); ++item)
    {
        // 依次比对校验
        AudioDeviceInfo *audioDeviceInfo = &(item->audioDeviceInfo);

        // 信息无误
        if (audioDeviceInfo->id == deviceList.value(audioDeviceInfo->name, "1"))
            continue;

        // 已经忽略，直接跳过
        if (mIgnoreMap->contains(audioDeviceInfo->id))
            continue;

        // id存在，说明名字变更,静默变更同步即可
        if (idList.contains(audioDeviceInfo->id))
        {
            qDebug() <<"任务:" << item->taskInfo.name << ", 设备名称存在变更:" << audioDeviceInfo->name << "->" << idToName.value(audioDeviceInfo->id);
            audioDeviceInfo->name = idToName.value(audioDeviceInfo->id);
            if (!mDatabase->updateItem(*item))
            {
                qCritical() << "Failed to update item:" << mDatabase->lastError();
                audioDeviceInfo->id = audioDeviceInfo->id;
                // 添加 id 到忽略列表
                mIgnoreMap->insert(audioDeviceInfo->id, 3);
            }
            continue;
        }

        // 名字存在，则说明id变更，静默变更同步即可
        if (nameList.contains(audioDeviceInfo->name))
        {
            qDebug() <<"任务:" << item->taskInfo.name << ", 设备ID存在变更:" << audioDeviceInfo->name;
            QString oldId = audioDeviceInfo->id;
            audioDeviceInfo->id = deviceList.value(audioDeviceInfo->name);
            if (!mDatabase->updateItem(*item))
            {
                qCritical() << "Failed to update item:" << mDatabase->lastError();
                audioDeviceInfo->id = oldId;
                // 添加 old id 到忽略列表
                mIgnoreMap->insert(audioDeviceInfo->id, 3);
            }
            continue;
        }

        // 两个信息都找不到，说明设备移除：询问是否需要修改关联项
        if (mIgnoreMap->value(audioDeviceInfo->id, 0) >= 10)
            continue;   // 只询问一次

        QString newName;
        QString newId;
        // 询问过
        if (changeDevice.contains(audioDeviceInfo->id))
        {
            newId = changeDevice.value(audioDeviceInfo->id);
            newName = idToName.value(newId);
        }
        // 未询问过
        else
        {
            qWarning() << "设备离线:" << audioDeviceInfo->name;
            QString text = QString("检测到原设备【%1】已移除\n\n"
                                   "是否将任务关联项替换为以下新设备？\n"
                                   "选择[取消]即忽略变更，在[设备重新接入]并[重启应用]后依旧生效。\n\n").arg(audioDeviceInfo->name);
            AudioChoiceDialog choiceDialog(text, mToolWidget);
            if (choiceDialog.exec() != QDialog::Accepted) {
                qInfo() << "离线设备" << audioDeviceInfo->name << "使用替换: 取消选择";
                // 添加 id 到忽略列表
                mIgnoreMap->insert(audioDeviceInfo->id, 10);
                continue;
            }

            AudioDeviceInfo* deviceInfo = choiceDialog.selectedOption();
            newName = deviceInfo->name;
            newId   = deviceInfo->id;
            qInfo() << "离线设备"  << audioDeviceInfo->name << "使用替换: " <<  deviceInfo->name;
        }

        QString oldId = audioDeviceInfo->id;
        audioDeviceInfo->name = newName;
        audioDeviceInfo->id   = newId;
        if (!mDatabase->updateItem(*item))
        {
            qCritical() << "Failed to update item:" << mDatabase->lastError();
            // 添加 id 到忽略列表
            mIgnoreMap->insert(audioDeviceInfo->id, 10);

            continue;
        }
        changeDevice[oldId] = newId;
    }
    qInfo() << "设备情况校验完成";
}
