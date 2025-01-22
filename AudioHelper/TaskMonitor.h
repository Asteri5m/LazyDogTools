#ifndef TASKMONITOR_H
#define TASKMONITOR_H

//win32 api
#include <Windows.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <Shlobj.h>

#include <QObject>
#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QDir>
#include <QThread>
#include <QMutex>
#include "AudioCustom.h"

typedef QList<TaskInfo> TaskInfoList;

inline QMutex taskMonitorMutex;

class TaskMonitor : public QObject
{
    Q_OBJECT
public:
    enum TaskMode {
        Process,
        Windows
    };
    Q_ENUM(TaskMode)

    enum FilterMode {
        All,
        Clear
    };
    Q_ENUM(FilterMode)

    explicit TaskMonitor(QObject *parent = nullptr);
    ~TaskMonitor();

    QStandardItemModel* getProcessModel();
    QStandardItemModel* getWindowsModel();
    QString filePath(const QModelIndex& index, TaskMode mode);
    void setFilter(QStringList& headers, TaskMode mode);
    void setFilter(FilterMode filterMode);

    static void getProcessList(TaskInfoList *taskInfoList);
    static void getWindowsList(TaskInfoList *taskInfoList);

public slots:
    void update();

private slots:
    void updateModel();

signals:
    void backgroundUpdate();

private:
    void updateProcessModel();
    void updateWindowsModel();
    bool filterProcess(QString& text);
    bool filterWindows(QString& text);
    static QString getExeDescription(const QString &filePath);

    QStandardItemModel* mProcessModel;
    QStandardItemModel* mWindowsModel;
    QFileInfoList *mProcessInfoList;
    QFileInfoList *mWindowsInfoList;
    QStringList *mProcessFilter;
    QStringList *mWindowsFilter;
    FilterMode mFilterMode;
    QThread *mThread;
};

#endif // TASKMONITOR_H
