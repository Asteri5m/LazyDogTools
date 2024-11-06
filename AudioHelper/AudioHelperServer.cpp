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
    mTimer->setInterval(500);
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
        calculateWindowsWeight();
        calculateProcessWeight();
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
    BOOL isDir = false;

    for(auto it = mTargetList->begin(); it != mTargetList->end(); it++)
    {
        uint key = *it;
        CHAR value = mTargetList->value(key);
        const RelatedItem *related = getRelated(key);
        if (value > targetWeight)
        {
            targetId = key;
            targetWeight = value;
            isDir = related->typeInfo.type == "文件夹";
        }

        // 降低文件夹的优先级但不降低权重
        if (value == targetWeight && isDir && related->typeInfo.type != "文件夹")
        {
            targetId = key;
            targetWeight = value;
            isDir = false;
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

    qInfo() << QString("任务触发：id:%1, weight:%2, name:%3, device:%4")
                    .arg(targetId)
                    .arg((short)targetWeight)
                    .arg(target->taskInfo.name)
                    .arg(target->audioDeviceInfo.name)
                    .toUtf8().constData();

    if (mAudioManager->setAudioOutDevice(target->audioDeviceInfo.id))
    {
        qDebug() << "执行完成";
    }
    else
    {
        qDebug() << "执行失败了";
    }

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

    // 对应刚打开的游戏，初始化需要一段时间，
    // 但是此时没有窗口，无法得到加权，导致部分程序初始化了错误的音频设备，
    // 并且该程序无法切换音频设备，那么此时就需要“预处理”，提取准备好音频设备
    QVector<uint> targetBuffer;
    for (auto it = taskInfoList.rbegin(); it != taskInfoList.rend(); ++it) {
        TaskInfo task = *it;

        // 仅补偿10s内打开的进程
        if (task.survivalTime > 10000)
            continue;

        QVector<uint> idList = searchRelateds(task.path);
        if (idList.isEmpty())
            continue;

        foreach (uint id, idList) {
            if (targetBuffer.contains(id))
                continue;

            if (mTargetList->contains(id))
                (*mTargetList)[id] += 2;
            else
                mTargetList->insert(id, 2);

            targetBuffer.append(id);
        }
    }

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
    QVector<QString> taskBuffer;
    QVector<uint> targetBuffer;
    for (auto it = taskInfoList->rbegin(); it != taskInfoList->rend(); ++it) {
        TaskInfo task = *it;

        // 避免多进程任务的重复加权
        if (taskBuffer.contains(task.path))
            continue;

        QVector<uint> idList = searchRelateds(task.path);
        if (idList.isEmpty())
            continue;

        foreach (uint id, idList) {
            if (targetBuffer.contains(id))
                continue;

            if (mTargetList->contains(id))
                (*mTargetList)[id] += weight;
            else
                mTargetList->insert(id, weight);

            targetBuffer.append(id);
        }
        taskBuffer.append(task.path);
    }
}


void AudioHelperServer::calculateSceneWeight()
{
    QString scene;
    switch (mScene) {
    case Scene::Normal:
        return;        // 普通模式不需要进行场景加权
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

QVector<uint> AudioHelperServer::searchRelateds(const QString path) const
{
    QVector<uint> reslut;
    for (const RelatedItem &item : *mRelatedList) {
        if (path.startsWith(item.taskInfo.path))
            reslut.append(item.id);
    }
    return reslut;
}

const RelatedItem *AudioHelperServer::getRelated(const uint id) const
{
    for (const RelatedItem &item : *mRelatedList) {
        if (item.id == id)
            return &item;
    }
    return nullptr;
}


