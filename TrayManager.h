#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <functional>

class TrayManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static TrayManager* instance();

    // 显示通知
    void showNotification(const QString& title, const QString& message);

    // 添加托盘菜单项，支持设置图标和子菜单
    void addMenuItem(const QString& name, std::function<void()> callback = nullptr, QMenu* parentMenu = nullptr, const QIcon& icon = QIcon());

    // 设置托盘的双击事件
    void setDoubleClick(std::function<void()> callback);

    // 添加子菜单
    QMenu* addSubMenu(const QString& name, QMenu* parentMenu = nullptr, const QIcon& icon = QIcon());

    // 添加分割线
    void addSeparator(QMenu* parentMenu = nullptr);

private:
    explicit TrayManager(QObject *parent = nullptr);
    static TrayManager* mInstance;  // 静态单例实例

    QSystemTrayIcon* mTrayIcon;
    QMenu* mTrayMenu;
};

#endif // TRAYMANAGER_H
