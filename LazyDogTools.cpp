/**
 * @file LazyDogTools.cpp
 * @author Asteri5m
 * @date 2025-02-07 15:44:32
 * @brief 程序主类
 */

#include <QSysInfo>
#include <QApplication>
#include "LazyDogTools.h"
#include "Settings.h"
#include "TrayManager.h"
#include "LogHandler.h"
#include "ToolManager.h"
#include "AudioHelper/AudioHelper.h"

LazyDogTools::LazyDogTools(QObject *parent)
    :QObject{ parent }
{
    QElapsedTimer timer;
    timer.start();
    qInfo() << "程序启动";
    qDebug() << "操作系统:" << QSysInfo::productType() << QSysInfo::productVersion();
    qDebug() << "系统架构:" << QSysInfo::currentCpuArchitecture();
    qDebug() << "内核版本:" << QSysInfo::kernelVersion();

    // 注册工具
    initTools();

    // 初始化设置
    mSettings = new Settings(this);
    connect(mSettings, SIGNAL(toolActiveChanged()), this, SLOT(trayUpdate()));

    // 初始化托盘
    initTray();

    TrayManager::instance().showMessage("程序启动成功", "欢迎使用，您的工具已准备就绪！");
    qInfo() << QString("程序加载完成 [%1ms]").arg(timer.elapsed()).toUtf8().constData();

    // 检查更新 - 如果开启了自动更新
    if (mSettings->loadConfig("自动更新") == "true")
    {
        mSettings->checkForUpdates();
    }
}


LazyDogTools::~LazyDogTools()
{
    qInfo() << "程序退出";
}

void LazyDogTools::onMessageAvailable(QString message)
{
    if (message == "Only one program instance is allowed to run.")
    {
        TrayManager::instance().showMessage("LazyDogTools", "LazyDogTools 已经在运行。");
        qInfo() << "Second instance refused.";
        return;
    }
    qWarning() << "Unknown message: " << message;
}

void LazyDogTools::initTools()
{
    ToolManager::instance().registerTool<AudioHelper>("音频助手",
                                                {"音频助手", ":/ico/audiohelper.svg", "一款根据场景自动切换音频设备的小助手",
                                                {"切换模式", "锁定设备", "切换场景"},
                                                true },
                                                [this]() { return new AudioHelper(this); });
}

void LazyDogTools::initTray()
{
    Settings *settings = mSettings;
    // 添加托盘菜单项
    TrayManager &trayManager = TrayManager::instance();
    trayManager.setMainAction([settings]() { settings->showWindow(); });
    trayManager.addMenuItem("首选项", [settings]() { settings->showWindow(); }, nullptr, QIcon(":/ico/settings2.svg"));
    trayManager.addSeparator();

    ToolManager& toolManager = ToolManager::instance();
    const ToolInfoMap& allToolsInfo = toolManager.getAllTools();
    for (auto it = allToolsInfo.begin(); it != allToolsInfo.end(); ++it)
    {
        if (it->enabled)
        {
            ToolModel *tool = toolManager.getCreatedTool(it.key());
            trayManager.addMenuItem(it->Name, [tool]() { tool->showWindow(); }, nullptr, QIcon(it->IconPath));
        }
    }

    // 添加固定项
    trayManager.addSeparator();
    trayManager.addMenuItem("检查更新", [settings]() { settings->checkForUpdates(); }, nullptr, QIcon(":/ico/loop.svg"));
    trayManager.addMenuItem("退出", []() { QApplication::exit(0); }, nullptr, QIcon(":/ico/close.svg"));
}

void LazyDogTools::trayUpdate()
{
    TrayManager::instance().clear();
    initTray();
}
