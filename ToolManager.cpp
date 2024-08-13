#include "ToolManager.h"


ToolManager::ToolManager(QObject *parent)
{
    initialize();
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

ShortList *ToolManager::getShortcut()
{
    return mShortcut;
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

void ToolManager::setShortcut(ShortList *shortcut)
{
    mShortcut = shortcut;
}

void ToolManager::setTray(TrayList *tray)
{
    mTray = tray;
}

void ToolManager::setActive(bool state)
{
    mActive = state;
}

void ToolManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

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

void ToolManager::deleteUI()
{
    // 避免多次释放
    if (mToolWidget == nullptr)
        return;

    delete(mToolWidget);
    mToolWidget = nullptr;
}

void ToolManager::initialize()
{
    setName("示例");
    setIcon(":/ico/LD.ico");
    setDescription("使用setXXX方法设置您的相关参数");
    setTray(nullptr);
    setShortcut(nullptr);
    setActive(true);
}
