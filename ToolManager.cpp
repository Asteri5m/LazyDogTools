#include "ToolManager.h"


ToolManager::ToolManager(QObject *parent)
{
    Q_UNUSED(parent);
    init();
}

QString ToolManager::getIcon()
{
    return mIcon;
}

QString ToolManager::getName()
{
    return mName;
}

QString ToolManager::getDescription()
{
    return mDescription;
}

HotkeyList *ToolManager::getHotKey()
{
    return mHotKeyList;
}

TrayList *ToolManager::getTrayn()
{
    return mTray;
}

bool ToolManager::getActive()
{
    return mActive;
}

void ToolManager::setIcon(QString icon)
{
    mIcon = icon;
}

void ToolManager::setName(QString name)
{
    mName = name;
}

void ToolManager::setDescription(QString desc)
{
    mDescription = desc;
}

void ToolManager::setHotKey(HotkeyList *hotKeyList)
{
    mHotKeyList = hotKeyList;
}

void ToolManager::setTray(TrayList *tray)
{
    mTray = tray;
}

void ToolManager::setActive(bool state)
{
    mActive = state;
    if (!state)
        deleteUI();
}

void ToolManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    qInfo() << "加载应用：" << getName();
    mToolWidget = new ToolWidgetModel();
}

void ToolManager::show()
{
    // 避免对空指针进行操作
    if (mToolWidget == nullptr)
        return;

    if (mToolWidget->isHidden())
        mToolWidget->showFirstPage();
    else if (mToolWidget->isMinimized())
        mToolWidget->showNormal();
    else
        mToolWidget->activateWindow();
}

void ToolManager::hide()
{
    // 避免对空指针进行操作
    if (mToolWidget == nullptr)
        return;
    mToolWidget->hide();
}

void ToolManager::init()
{
    initialize();
}

void ToolManager::deleteUI()
{
    // 避免多次释放
    if (mToolWidget == nullptr)
        return;

    qInfo() << "停用应用：" << getName();
    delete(mToolWidget);
    mToolWidget = nullptr;
}

void ToolManager::initialize()
{
    setName("示例");
    setIcon(":/ico/LD.ico");
    setDescription("使用setXXX方法设置您的相关参数");
    setTray(nullptr);
    setHotKey(nullptr);
    setActive(true);
}
