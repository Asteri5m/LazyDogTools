#include "Tool.h"


Tool::Tool(QObject *parent)
{
    initialize();
}

QString Tool::getIcon()
{
    return mIcon;
}

QString Tool::getName()
{
    return mName;
}

QString Tool::getDescription()
{
    return mDescription;
}

ShortList *Tool::getShortcut()
{
    return mShortcut;
}

TrayList *Tool::getTrayn()
{
    return mTray;
}

bool Tool::getActive()
{
    return mActive;
}

void Tool::setIcon(QString icon)
{
    mIcon = icon;
}

void Tool::setName(QString name)
{
    mName = name;
}

void Tool::setDescription(QString desc)
{
    mDescription = desc;
}

void Tool::setShortcut(ShortList *shortcut)
{
    mShortcut = shortcut;
}

void Tool::setTray(TrayList *tray)
{
    mTray = tray;
}

void Tool::setActive(bool state)
{
    mActive = state;
}

void Tool::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    mToolWidget = new ToolWidgetModel();
}

void Tool::show()
{
    // 避免对空指针进行操作
    if (mToolWidget == nullptr)
        return;

    if (mToolWidget->isHidden())
        mToolWidget->show();
    else
        mToolWidget->activateWindow();
}

void Tool::hide()
{
    // 避免对空指针进行操作
    if (mToolWidget == nullptr)
        return;
    mToolWidget->hide();
}

void Tool::deleteUI()
{
    // 避免多次释放
    if (mToolWidget == nullptr)
        return;

    delete(mToolWidget);
    mToolWidget = nullptr;
}

void Tool::initialize()
{
    setName("示例");
    setIcon(":/ico/LD.ico");
    setDescription("使用setXXX方法设置您的相关参数");
    setTray(nullptr);
    setShortcut(nullptr);
    setActive(true);
}
