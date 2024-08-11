#include "Settings.h"
#include "CustomWidget.h"


void Settings::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    mToolWidget = new SettingsWidget();
    mToolWidget->setWindowTitle(getName());
    mToolWidget->setWindowIcon(QIcon(getIcon()));
}


void Settings::initialize()
{
    setName("设置");
    setDescription("简单的设置");
}
