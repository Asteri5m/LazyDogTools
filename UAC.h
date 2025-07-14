#ifndef UAC_H
#define UAC_H

#include <ShlObj_core.h>
#include <Windows.h>
#include <shlobj.h>
#include <QCoreApplication>
#include <QDir>

// for IsUserAnAdmin()
#pragma comment(lib, "Shell32.lib")

class UAC
{
public:
    // 以管理员权限启动一个新实例
    // true-启动了新实例
    // false-未启动新实例
    static bool runAsAdmin()
    {
        if (IsUserAnAdmin())
        {
            return false; // 当前程序正以管理员权限运行
        }

        QStringList args = QCoreApplication::arguments(); // 获取命令行参数
        if (args.count() < 2 || args[1] != "runas")       // 不带参数或参数不为"runas"时，即直接运行
        {
            // 获取应用程序可执行文件的路径
            QString filePath = QCoreApplication::applicationFilePath();

            // 以管理员权限，执行exe程序
            HINSTANCE ins = ShellExecuteA(nullptr, "runas", filePath.toStdString().c_str(),
                                          "runas", nullptr, SW_SHOWNORMAL);
            if (ins > (HINSTANCE)32)
            {
                return true; // 程序新实例启动成功
            }
        }
        return false;
    }

    // 检查当前是否以管理员权限运行
    static bool isRunAsAdmin()
    {
        return IsUserAnAdmin();
    }

    static bool runArguments(const QString &arguments, bool asAdmin = false, const QString &program = QString(), bool waitForCompletion = false)
    {
        // 获取完整路径并处理空格
        QString fullPath = QDir::toNativeSeparators(
            program.isEmpty() ? QCoreApplication::applicationFilePath() : program
            );
        if (fullPath.contains(' ')) {
            fullPath = QString("\"%1\"").arg(fullPath);
        }
        QString safeArguments = arguments;
        if (arguments.contains(' ')) {
            safeArguments = QString("\"%1\"").arg(arguments);
        }

        QString workingDir = QFileInfo(program).absolutePath();

        // 安全转换
        std::wstring wFullPath = fullPath.toStdWString();
        std::wstring wArguments = safeArguments.toStdWString();
        std::wstring wWorkingDir = workingDir.toStdWString();

        // 设置执行信息
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.lpVerb = (asAdmin && !UAC::isRunAsAdmin()) ? L"runas" : L"open";
        sei.lpFile = wFullPath.c_str();
        sei.lpParameters = wArguments.c_str();
        sei.lpDirectory = wWorkingDir.c_str();
        sei.nShow = SW_NORMAL;
        sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;

        if (!ShellExecuteExW(&sei)) {
            DWORD error = GetLastError();
            QString errorMessage;
            switch (error) {
                case ERROR_CANCELLED:
                    errorMessage = "操作被用户取消";
                    break;
                case ERROR_FILE_NOT_FOUND:
                    errorMessage = "找不到指定的文件";
                    break;
                case ERROR_PATH_NOT_FOUND:
                    errorMessage = "找不到指定的路径";
                    break;
                case ERROR_ACCESS_DENIED:
                    errorMessage = "访问被拒绝";
                    break;
                default:
                    errorMessage = QString("未知错误 %1").arg(error);
            }
            qWarning() << "运行失败:" << errorMessage;
            return false;
        }

        if (sei.hProcess) {
            if (waitForCompletion) {
                WaitForSingleObject(sei.hProcess, INFINITE);
                DWORD exitCode = STILL_ACTIVE;
                GetExitCodeProcess(sei.hProcess, &exitCode);
                qDebug() << "进程退出代码:" << exitCode;
            }
            CloseHandle(sei.hProcess);
        }

        return true;
    }

    static bool setApplicationStartup(bool add, bool isArg = false)
    {
        PWSTR path = nullptr;
        // 获取所有用户的启动项目录
        HRESULT result = SHGetKnownFolderPath(FOLDERID_CommonStartup, 0, nullptr, &path);
        QString startupPath;
        if (SUCCEEDED(result))
        {
            startupPath = QString::fromWCharArray(path);
            CoTaskMemFree(path); // 释放内存
        }
        else
        {
            qWarning() << "获取启动项目录失败";
            return false;
        }

        QString appPath = QCoreApplication::applicationFilePath(); // 获取应用程序的路径
        QDir startupDir(startupPath);
        QString lnkFilePath = startupDir.filePath("LazyDogTools.lnk");

        if (add)
        {
            // 如果快捷方式已存在，说明已经设置了开机启动
            if (QFile::exists(lnkFilePath))
            {
                return true;
            }

            // 尝试创建快捷方式
            if (QFile::link(appPath, lnkFilePath))
            {
                return true;
            }

            // 如果是通过参数方式调用，直接返回失败
            if (isArg)
            {
                qWarning() << "创建开机启动快捷方式失败";
                return false;
            }

            // 尝试使用管理员权限重新创建
            qDebug() << "尝试使用管理员权限设置开机启动";
            return runArguments("-startup", true);
        }
        else
        { // 移除开机启动
            // 如果快捷方式不存在，说明已经没有开机启动
            if (!QFile::exists(lnkFilePath))
            {
                qDebug() << "开机启动已移除";
                return true;
            }

            // 尝试删除快捷方式
            bool removed = QFile::remove(lnkFilePath);
            if (removed)
            {
                qDebug() << "成功移除开机启动";
            }
            else
            {
                qWarning() << "移除开机启动失败";
            }
            return removed;
        }
    }
};

#endif // UAC_H
