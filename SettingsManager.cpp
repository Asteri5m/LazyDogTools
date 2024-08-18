#include "SettingsManager.h"
#include "CustomWidget.h"


void SettingsManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    mToolWidget = new Settings();
    mToolWidget->setWindowTitle(getName());
    mToolWidget->setWindowIcon(QIcon(getIcon()));
}


void SettingsManager::initialize()
{
    setName("设置");
    setDescription("简单的设置");
}

// 将工具窗口返回
Settings *SettingsManager::getTool()
{
    return (Settings *)mToolWidget;
}
