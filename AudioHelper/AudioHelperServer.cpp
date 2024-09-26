#include "AudioHelperServer.h"


AudioHelperServer::AudioHelperServer(RelatedList *relatedList, QObject *parent)
    : QObject{parent}
    , mRelatedList(relatedList)
    , mMode(Mode::Smart)
    , mScene(Scene::Normal)
    , mTimer(new QTimer)
    , mThread(new QThread)
    , mAudioManager(new AudioManager)
    , mTargetList(new WeightList)
{
    moveToThread(mThread);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(server()));
    mThread->start();

    // 服务的轮训的间隔默认为半秒
    mTimer->setInterval(1000);
}

AudioHelperServer::~AudioHelperServer()
{
    mThread->quit();
    mThread->wait();
}

void AudioHelperServer::setMode(const Mode mode)
{
    mMode = mode;
}

void AudioHelperServer::setScene(const Scene scene)
{
    mScene = scene;
}

AudioHelperServer::Mode AudioHelperServer::mode() const
{
    return mMode;
}

AudioHelperServer::Scene AudioHelperServer::scene() const
{
    return mScene;
}

void AudioHelperServer::start()
{
    mTimer->start();
}

void AudioHelperServer::stop()
{
    mTimer->stop();
}

void AudioHelperServer::setTimer(const uint msec)
{
    mTimer->setInterval(msec);
}

void AudioHelperServer::server()
{
    if(!audioServerMutex.try_lock())
        return;

    mTargetList->clear();

    // 计算初始的权重
    switch (mMode) {
    case Mode::Smart:
        calculateProcessWeight();
        calculateWindowsWeight();
        break;
    case Mode::Process:
        calculateProcessWeight();
        break;
    case Mode::Windows:
        calculateWindowsWeight();
        break;
    default:
        audioServerMutex.unlock();
        return;
    }

    // 计算特殊场景加权
    calculateSceneWeight();

    uint targetId = -1;
    CHAR targetWeight = 0;

    for(auto it = mTargetList->begin(); it != mTargetList->end(); it++)
    {
        uint key = *it;
        CHAR value = mTargetList->value(key);
        if (value > targetWeight)
        {
            targetId = key;
            targetWeight = value;
        }
    }

    // 未匹配到任何目标
    if (targetId < 0 || targetWeight == 0)
    {
        audioServerMutex.unlock();
        return;
    }

    // 根据id进行检索目标项
    const RelatedItem *target = getRelated(targetId);

    // 进行切换---设备目标一致的情况下不进行切换
    if (target->audioDeviceInfo.id == mAudioManager->getCurrentAudioOutDevice())
    {
        audioServerMutex.unlock();
        return;
    }
    mAudioManager->setAudioOutDevice(target->audioDeviceInfo.id);

    qDebug() << QString("任务触发：id:%1, weight:%2, name:%3, device:%4")
                    .arg(targetId)
                    .arg((short)targetWeight)
                    .arg(target->taskInfo.name)
                    .arg(target->audioDeviceInfo.name)
                    .toUtf8().constData();

    audioServerMutex.unlock();
}

void AudioHelperServer::calculateProcessWeight()
{
    TaskInfoList taskInfoList = TaskMonitor::getProcessList();
    if (taskInfoList.isEmpty())
    {
        qWarning() << "获取进程信息失败！";
        return;
    }

    calculateWeight(&taskInfoList, 1);

}

void AudioHelperServer::calculateWindowsWeight()
{
    TaskInfoList taskInfoList = TaskMonitor::getWindowsList();
    if (taskInfoList.isEmpty())
    {
        qWarning() << "获取窗口信息失败！";
        return;
    }

    calculateWeight(&taskInfoList, 2);
}

void AudioHelperServer::calculateWeight(TaskInfoList* taskInfoList, char weight)
{
    QVector<QString> buffer;
    for (auto it = taskInfoList->rbegin(); it != taskInfoList->rend(); ++it) {
        TaskInfo task = *it;

        // 避免多进程任务的重复加权
        if (buffer.contains(task.path))
            continue;

        LONG id = searchRelated(task.path);
        if (id < 0)
            continue;

        if (mTargetList->contains(id))
            (*mTargetList)[id] += weight;
        else
            mTargetList->insert(id, weight);

        buffer.append(task.path);
    }
}


void AudioHelperServer::calculateSceneWeight()
{
    QString scene;
    switch (mScene) {
    case Scene::Normal:
        return;     // 普通模式不需要进行场景加权
    case Scene::Audiovisual:
        scene = "影音";
        break;
    case Scene::Entertainment:
        scene = "游戏";
        break;
    default:
        return;
    }

    for(auto it = mTargetList->begin(); it != mTargetList->end(); it++)
    {
        uint key = *it;
        const RelatedItem *related = getRelated(key);
        if (related->typeInfo.tag == scene)
            (*mTargetList)[key] += 1;
    }
}

LONG AudioHelperServer::searchRelated(const QString path) const
{
    for (const RelatedItem &item : *mRelatedList) {
        if (item.taskInfo.path == path)
            return item.id;
    }
    return -1;
}

const RelatedItem *AudioHelperServer::getRelated(const uint id) const
{
    for (const RelatedItem &item : *mRelatedList) {
        if (item.id == id)
            return &item;
    }
    return nullptr;
}


