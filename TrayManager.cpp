/**
 * @file TrayManager.cpp
 * @author Asteri5m
 * @date 2025-02-07 15:19:20
 * @brief 托盘单例，封装QSystemTrayIcon用于全局调用
 */

#include "TrayManager.h"
#include <QAction>
#include <QApplication>


TrayManager::TrayManager(QObject *parent)
    : QObject(parent),
    mTrayIcon(new QSystemTrayIcon(this)),
    mTrayMenu(new QMenu())
{
    // 设置托盘图标
    mTrayIcon->setIcon(QIcon(":/ico/LD.ico"));
    mTrayIcon->setToolTip("LazyDogTools");
    mTrayIcon->setContextMenu(mTrayMenu);
    mTrayIcon->show();
}

TrayManager& TrayManager::instance()
{
    static TrayManager instance;
    return instance;
}

void TrayManager::addMenuItem(const QString& name, std::function<void()> callback, QMenu* parentMenu, const QIcon& icon)
{
    QMenu* menu = parentMenu ? parentMenu : mTrayMenu;
    QAction* action = new QAction(icon, name, menu);

    if (callback) {
        connect(action, &QAction::triggered, this, [callback]() {
            callback();
        });
    }

    menu->addAction(action);
}

/**
 * @brief 设置托盘图标的主操作（左键双击回调）
 * @param callback 当用户左键双击托盘图标时执行的回调函数
 */
void TrayManager::setMainAction(std::function<void ()> callback)
{
    // 确保只有一个连接
    disconnect(mTrayIcon, &QSystemTrayIcon::activated, this, nullptr);

    connect(mTrayIcon, &QSystemTrayIcon::activated, this, [callback](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            callback();
        }
    });
}

QMenu* TrayManager::addSubMenu(const QString& name, QMenu* parentMenu, const QIcon& icon)
{
    QMenu* menu = parentMenu ? parentMenu : mTrayMenu;
    QMenu* subMenu = new QMenu(name, menu);
    subMenu->setIcon(icon);
    menu->addMenu(subMenu);
    return subMenu;
}

void TrayManager::addSeparator(QMenu* parentMenu)
{
    QMenu* menu = parentMenu ? parentMenu : mTrayMenu;
    menu->addSeparator();
}

void TrayManager::clear()
{
    mTrayMenu->clear();
}

void TrayManager::showMessage(const QString &title, const QString &msg, const QIcon &icon, int msecs)
{
    mTrayIcon->showMessage(title, msg, icon, msecs);
}

void TrayManager::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msecs)
{
    mTrayIcon->showMessage(title, msg, icon, msecs);
}
