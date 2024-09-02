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

    HotkeyList *hotKeyList = new HotkeyList();
    hotKeyList->append(HotKey({"切换设置", QKeySequence("Ctrl+Alt+C"),nullptr}));
    hotKeyList->append(HotKey({"锁定设备", QKeySequence(),nullptr}));
    hotKeyList->append(HotKey({"切换模式", QKeySequence(),nullptr}));
    setHotKey(hotKeyList);
}
