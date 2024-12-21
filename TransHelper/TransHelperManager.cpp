#include "TransHelperManager.h"
#include "TransHelper.h"


void TransHelperManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    qInfo() << "加载应用:" << getName();
    mToolWidget = new TransHelper();
    mToolWidget->setWindowTitle(getName());
    mToolWidget->setWindowIcon(QIcon(getIcon()));
}

void TransHelperManager::initialize()
{
    setName("翻译助手");
    setDescription("简介");

    HotkeyList *hotKeyList = new HotkeyList();
    hotKeyList->append(HotKey({"打开搜索", QKeySequence(),nullptr}));
    hotKeyList->append(HotKey({"复习上一个", QKeySequence(),nullptr}));
    hotKeyList->append(HotKey({"暂停/开启", QKeySequence(),nullptr}));
    hotKeyList->append(HotKey({"复习下一个", QKeySequence(),nullptr}));
    setHotKey(hotKeyList);
}
