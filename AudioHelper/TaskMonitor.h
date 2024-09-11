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

class TaskMonitor : public QObject
{
    Q_OBJECT
public:
    enum TaskMode {
        Process,
        Windows
    };
    Q_ENUM(TaskMode)

    explicit TaskMonitor(QObject *parent = nullptr);
    ~TaskMonitor();

    QStandardItemModel* getProcessModel();
    QStandardItemModel* getWindowsModel();
    QString filePath(const QModelIndex& index, TaskMode mode);
    void setFilter(QStringList& headers, TaskMode mode);

public slots:
    void update();

private:
    void updateProcessModel();
    void updateWindowsModel();
    bool filterProcess(QString& text);
    bool filterWindows(QString& text);
    QString getExeDescription(const QString &filePath);

    QStandardItemModel* mProcessModel;
    QStandardItemModel* mWindowsModel;
    QFileInfoList *mProcessInfoList;
    QFileInfoList *mWindowsInfoList;
    QStringList *mProcessFilter;
    QStringList *mWindowsFilter;
};

#endif // TASKMONITOR_H
