#include <QSysInfo>
#include "LazyDogTools.h"
#include "ui_lazydogtools.h"
#include "SettingsManager.h"
#include "TrayManager.h"
#include "AudioHelper/AudioHelperManager.h"
#include "TransHelper/TransHelperManager.h"
#include <QApplication>

LazyDogTools::LazyDogTools(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LazyDogTools)
{
    QElapsedTimer timer;
    timer.start();
    qInfo() << "程序启动";
    qDebug() << "操作系统:" << QSysInfo::productType() << QSysInfo::productVersion();
    qDebug() << "系统架构:" << QSysInfo::currentCpuArchitecture();
    qDebug() << "内核版本:" << QSysInfo::kernelVersion();

    ui->setupUi(this);

    setWindowIcon(QIcon(":/ico/LD.ico"));

    // "设置"属于LazyDogTools的一部分，而不是应用"模块"
    SettingsManager* settingsManager = new SettingsManager();
    settingsManager->initialize();  // 手动初始化
    settingsManager->initUI();
    Settings* settings = settingsManager->getTool();

    // 设置中可以关闭不需要的工具，那么就需要更新主界面
    connect(settings, SIGNAL(appActiveChanged()), this, SLOT(updateUI()));

    mToolManagerList.append(settingsManager);
    mToolManagerList.append(new AudioHelperManager());
    mToolManagerList.append(new TransHelperManager());

    // 添加托盘菜单项
    TrayManager* trayManager = TrayManager::instance();
    trayManager->setTriggered([this]() { this->show(); this->activateWindow(); } );
    trayManager->addMenuItem("主界面", [this]() { this->show(); this->activateWindow(); }, nullptr, QIcon(":/ico/home.svg"));
    trayManager->addMenuItem("设置", [settings]() { settings->show(); settings->activateWindow(); }, nullptr, QIcon(":/ico/settings2.svg"));
    trayManager->addSeparator();

    initUI();
    // 将应用管理器的列表传递给“设置”,完成settings剩余的初始化工作
    settings->setToolManagerList(&mToolManagerList);

    trayManager->addSeparator();
    trayManager->addMenuItem("检查更新", []() { }, nullptr, QIcon(":/ico/loop.svg"));
    trayManager->addMenuItem("退出", []() { QApplication::exit(); }, nullptr, QIcon(":/ico/close.svg"));

    trayManager->showNotification("程序启动成功", "欢迎使用，您的工具已准备就绪！");
    qInfo() << QString("程序加载完成 [%1ms]").arg(timer.elapsed()).toUtf8().constData();
}

LazyDogTools::~LazyDogTools()
{
    qInfo() << "程序退出";
    delete ui;
}

QString LazyDogTools::loadSetting(const QString &key)
{
    Settings* settings = qobject_cast<SettingsManager*>(mToolManagerList.at(0))->getTool();

    return settings->loadSetting(key);
}

/**
 * 初始化程序的数据结构，解析各应用的数据
*/
void LazyDogTools::initData()
{
    TrayManager* trayManager = TrayManager::instance();
    Settings* settings = qobject_cast<SettingsManager*>(mToolManagerList.at(0))->getTool();
    short id = 0;
    for(auto toolManager : mToolManagerList)
    {
        toolManager->initialize();

        if (id > 0)
        {
            QMenu* subMenu = trayManager->addSubMenu(toolManager->getName(), nullptr, QIcon(toolManager->getIcon()));
            trayManager->addMenuItem("打开", [toolManager]() { toolManager->show(); }, subMenu, QIcon(":/ico/terminal.svg"));

            TrayList *trayList = toolManager->getTray();
            if (trayList)
                for (const TrayItem& trayItem : *trayList)
                    trayManager->addMenuItem(trayItem.Name, trayItem.Func, subMenu, QIcon(trayItem.Icon));
        }

        bool active = settings->loadSetting("active:"+toolManager->getName(), "true") == "true";
        toolManager->setActive(active);
        if (toolManager->getActive())
        {
            toolManager->initUI();
            mMinToolList.append(MinToolListItem({
                id,
                toolManager->getIcon(),
                toolManager->getName(),
                toolManager->getDescription(),
                toolManager
            }));
            id++; // id 自增
        }
    }
}

/**
 * 初始化UI
*/
void LazyDogTools::initUI()
{
    initData(); //初始化数据

    // 创建容器控件
    mWidget = new QWidget(this);
    mLayout = new QVBoxLayout(mWidget);

    for (const auto &minTool : mMinToolList)
    {
        MinToolWidget *widget = new MinToolWidget(minTool.id, minTool.icon, minTool.name, minTool.description);
        ToolManager* tool = minTool.tool;
        mLayout->addWidget(widget);
        connect(widget, SIGNAL(widgetDoubleClicked()), tool, SLOT(show()));
    }
    // 添加一个填充，以便于在Tool较少时撑起空间
    mLayout->addStretch();

    // 创建 QScrollArea 实例
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(mWidget);

    // 隐藏滚动条
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //布局设置
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(scrollArea);
    layout->setContentsMargins(0, 0, 0, 0); // 去除容器布局的边距

    ui->widget->setLayout(layout);
}

void LazyDogTools::updateUI()
{
    // 清空原来的内容
    QLayoutItem* item;
    while ((item = mLayout->takeAt(0)) != nullptr)
    {
        mLayout->removeItem(item);
        if (item->widget())
            delete item->widget();
        else
            delete item;
    }

    // 更新数据
    updateData();

    // 绘制新内容
    for (const auto &minTool : mMinToolList)
    {
        MinToolWidget *widget = new MinToolWidget(minTool.id, minTool.icon, minTool.name, minTool.description);
        ToolManager* tool = minTool.tool;
        mLayout->addWidget(widget);
        connect(widget, SIGNAL(widgetDoubleClicked()), tool, SLOT(show()));
    }
    // 添加一个垂直间隔，以便于在Tool较少时撑起空间
    mLayout->addStretch();
}

void LazyDogTools::onMessageAvailable(QString message)
{
    if (message == "Only one program instance is allowed to run.") {
        TrayManager::instance()->showNotification("LazyDogTools", "LazyDogTools 已经在运行。");
        qInfo() << "Second instance refused.";
        return;
    }
    qWarning() << "Unknown message：" << message;
}

void LazyDogTools::updateData()
{
    mMinToolList.clear();
    short id = 0;
    for(auto toolManager:mToolManagerList)
    {
        if (toolManager->getActive())
        {
            toolManager->initUI();
            mMinToolList.append(MinToolListItem({
                id,
                toolManager->getIcon(),
                toolManager->getName(),
                toolManager->getDescription(),
                toolManager
            }));
            id++; // id 自增
        }
    }
}

void LazyDogTools::closeEvent(QCloseEvent *event)
{
    if(loadSetting("关闭设置") == "最小化托盘"){
        event->ignore();
        hide();
    } else {
        QApplication::exit();
    }
}
