#include "AudioHelperManager.h"
#include "AudioHelper.h"


void AudioHelperManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    qInfo() << "加载应用: " << getName();
    mToolWidget = new AudioHelper();
    mToolWidget->setWindowTitle(getName());
    mToolWidget->setWindowIcon(QIcon(getIcon()));
}

void AudioHelperManager::initialize()
{
    setName("音频助手");
    setDescription("一个简单的助手");

    HotkeyList *hotKeyList = new HotkeyList();
    hotKeyList->append(HotKey({"切换模式", QKeySequence("Ctrl+Alt+C"), nullptr}));
    hotKeyList->append(HotKey({"锁定设备", QKeySequence("Ctrl+Alt+Z"), nullptr}));
    hotKeyList->append(HotKey({"切换场景", QKeySequence("Ctrl+Alt+M"), nullptr}));
    setHotKey(hotKeyList);

    TrayList *trayList = new TrayList();
    trayList->append(TrayItem({"切换模式", ":/ico/dashboard.svg", nullptr}));
    trayList->append(TrayItem({"切换场景", ":/ico/gamepad.svg", nullptr}));
    trayList->append(TrayItem({"锁定设备", ":/ico/pushpin.svg", nullptr}));
    setTray(trayList);
}
