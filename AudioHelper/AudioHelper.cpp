#include "AudioHelper.h"
#include "AudioHelperWedgit.h"


void AudioHelper::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    mToolWidget = new AudioHelperWedgit();
}

void AudioHelper::initialize()
{
    setName("音频助手");
    setDescription("一个简单的助手");
}