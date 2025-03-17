#ifndef TOOLMODEL_H
#define TOOLMODEL_H

/**
 * @file ToolModel.h
 * @author Asteri5m
 * @date 2025-02-07 17:28:58
 * @brief 工具的模版，建议所有的工具都继承此类
 */

#include <QObject>
#include <QKeySequence>
#include "CustomWidget.h"

typedef std::function<void()> Function;

struct TrayItem {
    QString     Name;            // 条目名，在菜单中显示
    QString     Icon;            // 图标路径
    Function    Func;            // 回调函数
};

// 菜单栏条目，菜单名:关联函数
typedef QList<TrayItem> TrayList;
// 配置 key：value
typedef QMap<QString, QString> Config;

class ToolModel : public QObject {
    Q_OBJECT

public:
    explicit ToolModel(QObject *parent = nullptr);
    ~ToolModel();

    // 获取数据接口
    TrayList*   getTray();

    // 修改数据接口
    void setTray(TrayList*  tray);

public slots:
    virtual void showWindow();
    virtual void toolWindowClosed();
    virtual void toolWindowEvent(const QString &type, const QString &context);
    virtual void hotKeyEvent(const QString &event);

private:
    TrayList*   mTray;          // 托盘菜单项

protected:
    ToolWidgetModel* mToolWidget;
};

#endif // TOOLMODEL_H
