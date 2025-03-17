#ifndef AUDIOHELPERSERVER_H
#define AUDIOHELPERSERVER_H

/**
 * @file AudioHelperServer.h
 * @author Asteri5m
 * @date 2025-02-10 22:20:10
 * @brief 音频助手后台服务，核心功能实现
 */

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QMutex>

#include "TaskMonitor.h"
#include "AudioManager.h"
#include "Custom.h"

inline QMutex audioServerMutex;

// key:RelatedItem->id, value:Weight
typedef OrderedMap<UINT, CHAR> WeightList;

class AudioHelperServer : public QObject
{
    Q_OBJECT
public:
    enum Mode {
        Process,
        Windows,
        Smart
    };
    Q_ENUM(Mode)

    enum Scene {
        Normal,
        Entertainment,
        Audiovisual
    };
    Q_ENUM(Scene)

    explicit AudioHelperServer(RelatedList *relatedList, QObject *parent = nullptr);
    ~AudioHelperServer();

    void setMode(const Mode mode);
    void setScene(const Scene scene);
    void setNotify(const bool notify);

    Mode mode() const;
    Scene scene() const;
    bool notify() const;
    bool state() const;

    void setTimer(const uint msec);

public slots:
    void start();
    void stop();

private slots:
    void server();

private:
    Mode mMode;
    Scene mScene;
    bool mNotify;
    bool mState;
    QTimer *mTimer;
    QThread *mThread;
    AudioManager *mAudioManager;
    RelatedList *mRelatedList;
    WeightList *mTargetList;
    QMutex mMutex;

    void calculateProcessWeight();
    void calculateWindowsWeight();
    void calculateSceneWeight();
    void calculateWeight(TaskInfoList *taskInfoList, char weight);
    QVector<uint> searchRelateds(const QString path) const;
    const RelatedItem *getRelated(const uint id) const;
};

#endif // AUDIOHELPERSERVER_H
