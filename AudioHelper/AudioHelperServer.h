#ifndef AUDIOHELPERSERVER_H
#define AUDIOHELPERSERVER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QMutex>

#include "TaskMonitor.h"
#include "AudioManager.h"

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

    enum Scene {
        Normal,
        Entertainment,
        Audiovisual
    };

    explicit AudioHelperServer(RelatedList *relatedList, QObject *parent = nullptr);
    ~AudioHelperServer();

    void setMode(const Mode mode);
    void setScene(const Scene scene);

    Mode mode() const;
    Scene scene() const;

    void setTimer(const uint msec);

public slots:
    void start();
    void stop();

private slots:
    void server();

private:
    Mode mMode;
    Scene mScene;
    QTimer *mTimer;
    QThread *mThread;
    AudioManager *mAudioManager;
    RelatedList *mRelatedList;
    WeightList *mTargetList;

    void calculateProcessWeight();
    void calculateWindowsWeight();
    void calculateSceneWeight();
    void calculateWeight(TaskInfoList *taskInfoList, char weight);
    LONG searchRelated(const QString path) const;
    const RelatedItem *getRelated(const uint id) const;
};

#endif // AUDIOHELPERSERVER_H
