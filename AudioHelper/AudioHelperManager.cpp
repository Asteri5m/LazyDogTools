#include "AudioHelperManager.h"
#include "AudioHelper.h"


void AudioHelperManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    mToolWidget = new AudioHelper();
    mToolWidget->setWindowTitle(getName());
    mToolWidget->setWindowIcon(QIcon(getIcon()));
}

void AudioHelperManager::initialize()
{
    setName("音频助手");
    setDescription("一个简单的助手");
}
