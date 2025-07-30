#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

/**
 * @file AudioHelper.h
 * @author Asteri5m
 * @date 2025-02-08 0:11:12
 * @brief 音频助手，主要功能为根据预设切换音频设备
 */

#include "ToolModel.h"
#include "AudioHelperWidget.h"
#include "AudioHelperServer.h"
#include "AudioDatabase.h"

class AudioHelper : public ToolModel
{
    Q_OBJECT
public:
    AudioHelper(QObject *parent=nullptr);
    ~AudioHelper();

    RelatedList *mRelatedList;
    IgnoreMap *mIgnoreMap;

public slots:
    void showWindow();
    void saveConfig(const QString &key, const QString &value);
    void hotKeyEvent(const QString &event);

private:
    AudioHelperServer *mServer;
    AudioDatabase *mDatabase;
    Config *mConfig;
    QMap<QString, AudioHelperServer::Mode> *mModeMap;
    QMap<QString, AudioHelperServer::Scene> *mSceneMap;

    void nextMode();
    void nextScene();
    void lockDevice();
    void checkRelateds();
};

#endif // AUDIOHELPER_H
