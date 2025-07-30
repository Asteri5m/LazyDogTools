#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

/**
 * @file TrayManager.h
 * @author Asteri5m
 * @date 2025-02-07 15:19:20
 * @brief 托盘单例，封装QSystemTrayIcon用于全局调用
 */

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <functional>

class TrayManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static TrayManager &instance();

    // 添加托盘菜单项，支持设置图标和子菜单
    void addMenuItem(const QString& name, std::function<void()> callback = nullptr, QMenu* parentMenu = nullptr, const QIcon& icon = QIcon());

    // 设置托盘的双击事件
    void setMainAction(std::function<void()> callback);

    // 添加子菜单
    QMenu* addSubMenu(const QString& name, QMenu* parentMenu = nullptr, const QIcon& icon = QIcon());

    // 添加分割线
    void addSeparator(QMenu* parentMenu = nullptr);

    // 清除所有子项
    void clear();

public Q_SLOTS:
    // 接口转发
    void showMessage(const QString &title, const QString &msg, const QIcon &icon, int msecs = 3000);
    void showMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 3000);

private:
    explicit TrayManager(QObject *parent = nullptr);

    QSystemTrayIcon* mTrayIcon;
    QMenu* mTrayMenu;
};

#endif // TRAYMANAGER_H
