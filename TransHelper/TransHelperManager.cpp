#include "TransHelperManager.h"
#include "TransHelper.h"


void TransHelperManager::initUI()
{
    // 避免多次创建
    if (mToolWidget != nullptr)
        return;

    mToolWidget = new TransHelper();
    mToolWidget->setWindowTitle(getName());
    mToolWidget->setWindowIcon(QIcon(getIcon()));
}

void TransHelperManager::initialize()
{
    setName("翻译助手");
    setIcon(":/ico/LD.ico");
    setDescription("简介");
    setTray(nullptr);
    setShortcut(nullptr);
    setActive(true);
}

// void TransHelperManager::show()
// {
//     // 避免对空指针进行操作
//     if (mToolWidget == nullptr)
//         return;

//     if (mToolWidget->isHidden())
//         mToolWidget->show();
//     else
//         mToolWidget->activateWindow();
// }
