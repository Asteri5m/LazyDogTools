#include "TaskMonitor.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// 构造函数
TaskMonitor::TaskMonitor(QObject *parent)
    : QObject(parent),
    mProcessModel(new QStandardItemModel(this)),
    mWindowsModel(new QStandardItemModel(this)),
    mProcessInfoList(new QFileInfoList),
    mWindowsInfoList(new QFileInfoList),
    mProcessFilter(new QStringList()),
    mWindowsFilter(new QStringList())
{

}

// 析构函数
TaskMonitor::~TaskMonitor()
{
    delete mProcessInfoList;
    delete mWindowsInfoList;
}

// 获取进程模型
QStandardItemModel* TaskMonitor::getProcessModel()
{
    return mProcessModel;
}

// 获取窗口模型
QStandardItemModel* TaskMonitor::getWindowsModel()
{
    return mWindowsModel;
}

// 根据模型索引和模式获取文件路径
QString TaskMonitor::filePath(const QModelIndex& index, TaskMode mode)
{
    if (mode == Process && index.isValid()) {
        return mProcessInfoList->at(index.row()).absoluteFilePath();
    } else if (mode == Windows && index.isValid()) {
        return mWindowsInfoList->at(index.row()).absoluteFilePath();
    }
    return QString();
}

void TaskMonitor::setFilter(QStringList &headers, TaskMode mode)
{
    switch (mode) {
    case Process:
        mProcessFilter = new QStringList(headers);
        break;
    case Windows:
        mWindowsFilter = new QStringList(headers);
    }
}

// 更新数据（更新模型）
void TaskMonitor::update()
{
    updateProcessModel();
    updateWindowsModel();
}

// 更新进程模型
void TaskMonitor::updateProcessModel()
{
    // 枚举进程
    DWORD processes[1024], processCount, cbNeeded;
    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        qDebug() << "Failed to enumerate processes.";
        return;
    }

    // 清空模型和进程信息列表
    mProcessModel->clear();
    mProcessInfoList->clear();

    processCount = cbNeeded / sizeof(DWORD);

    // 遍历所有进程
    for (unsigned int i = 0; i < processCount; ++i) {
        DWORD processId = processes[i];

        // 打开进程
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess == nullptr)
            continue;

        // 获取进程的可执行文件路径
        TCHAR processPath[MAX_PATH];
        DWORD size = sizeof(processPath) / sizeof(TCHAR);
        if (!QueryFullProcessImageName(hProcess, 0, processPath, &size))
            continue;

        QString drivepath = QDir::cleanPath(QString::fromWCharArray(processPath));

        // 过滤
        if (!filterProcess(drivepath))
            continue;

        // 将进程信息添加到文件信息列表（模拟 QFileInfo 用于兼容接口）
        QFileInfo fileInfo(drivepath);
        if (mProcessInfoList->contains(fileInfo))
            continue;

        mProcessInfoList->append(fileInfo);

        // 获取friendname, 首先尝试解析，解析失败后则使用QFileInfo::baseName
        QString friendName = getExeDescription(drivepath);
        if (friendName == "")
            friendName = fileInfo.baseName();

        QFileIconProvider iconProvider;
        QStandardItem *item = new QStandardItem(iconProvider.icon(fileInfo), friendName);
        mProcessModel->appendRow(item);
    }
    qDebug() << "Enumerate process number：" << mProcessInfoList->length();
}

// 获取friendname
QString TaskMonitor::getExeDescription(const QString& filePath)
{
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSize((LPCWSTR)filePath.utf16(), &handle);
    if (size == 0) {
        return QString();
    }

    QByteArray buffer(size, 0);
    if (!GetFileVersionInfo((LPCWSTR)filePath.utf16(), handle, size, buffer.data())) {
        return QString();
    }

    VS_FIXEDFILEINFO* fileInfo = nullptr;
    UINT fileInfoSize = 0;
    if (!VerQueryValue(buffer.data(), L"\\", (LPVOID*)&fileInfo, &fileInfoSize)) {
        return QString();
    }

    // 获取语言和代码页
    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *translate;
    UINT translateSize = 0;
    if (!VerQueryValue(buffer.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&translate, &translateSize)) {
        return QString();
    }

    // 构建查询路径
    QString queryString = QString("\\StringFileInfo\\%1%2\\FileDescription")
                              .arg(QString::number(translate->wLanguage, 16), 4, QLatin1Char('0'))
                              .arg(QString::number(translate->wCodePage, 16), 4, QLatin1Char('0'));

    // 查询文件描述
    LPVOID description = nullptr;
    UINT sizeDescription = 0;
    if (!VerQueryValue(buffer.data(), (LPCWSTR)queryString.utf16(), &description, &sizeDescription)) {
        return QString();
    }

    return QString::fromWCharArray((WCHAR*)description);
}

void TaskMonitor::updateWindowsModel()
{
    // 清空模型和窗口信息列表
    mWindowsModel->clear();
    mWindowsInfoList->clear();

    // 使用 lambda 表达式作为 EnumWindows 的回调
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        TaskMonitor *monitor = reinterpret_cast<TaskMonitor *>(lParam);
        if (!IsWindowVisible(hwnd))
            return TRUE;

        TCHAR windowTitle[256];
        GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(TCHAR));
        QString title = QString::fromWCharArray(windowTitle);
        if (title.isEmpty())
            return TRUE;

        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (!processHandle)
            return TRUE;

        WCHAR executablePath[MAX_PATH];
        DWORD pathSize = MAX_PATH;
        if (!QueryFullProcessImageNameW(processHandle, 0, executablePath, &pathSize)) {
            CloseHandle(processHandle);
            return TRUE;
        }

        QString drivepath = QDir::cleanPath(QString::fromWCharArray(executablePath));

        // 过滤
        if (!monitor->filterWindows(drivepath))
            return TRUE;

        // 将进程信息添加到文件信息列表,因为是递归查找窗口，所以是逆序的，需要将数据往头部插入
        QFileInfo fileInfo(drivepath);
        monitor->mWindowsInfoList->insert(0,fileInfo);

        QFileIconProvider iconProvider;
        QStandardItem *item = new QStandardItem(iconProvider.icon(fileInfo), title);
        monitor->mWindowsModel->insertRow(0, item);

        CloseHandle(processHandle);
        return TRUE;
    }, reinterpret_cast<LPARAM>(this));

    qDebug() << "Enumerate windows number：" << mWindowsInfoList->length();
}

bool TaskMonitor::filterProcess(QString &text)
{
    foreach (QString filter , *mProcessFilter)
    {
        if (text.startsWith(filter))
            return false;
    }

    return true;
}

bool TaskMonitor::filterWindows(QString &text)
{
    foreach (QString filter , *mWindowsFilter)
    {
        if (text.startsWith(filter))
            return false;
    }

    return true;
}
