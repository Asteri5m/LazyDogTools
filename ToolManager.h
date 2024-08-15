#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QShortcut>
#include "CustomWidget.h"

struct ShortCut {
    QString    Name;            // 快捷键名，会在设置中展示
    QShortcut* Shortkeys;       // 快捷键内容
    void*      Func;            // 需要与快捷键关联的函数
};

// 快捷键列表
typedef QList<ShortCut>      ShortList;
// 菜单栏条目，菜单名:关联函数
typedef QMap<QString, void*> TrayList;

class ToolManager : public QObject {
    Q_OBJECT

public:
    explicit ToolManager(QObject *parent = nullptr);

    // 获取数据接口
    QString     getIcon();
    QString     getName();
    QString     getDescription();
    ShortList*  getShortcut();
    TrayList*   getTrayn();
    bool        getActive();

    // 修改数据接口
    void setIcon(QString icon);
    void setName(QString name);
    void setDescription(QString desc);
    void setShortcut(ShortList* shortcut);
    void setTray(TrayList*  tray);
    void setActive(bool state);

    virtual void initUI();
    virtual void deleteUI();
    virtual void initialize();

public slots:
    virtual void show();
    virtual void hide();

private:
    QString    mIcon;          // 图标
    QString    mName;          // 应用名
    QString    mDescription;   // 应用描述
    ShortList* mShortcut;      // 快捷键列表
    TrayList*  mTray;          // 托盘菜单项
    bool       mActive;        // 工具启用状态

    void init();

protected:
    ToolWidgetModel* mToolWidget;

};

#endif // TOOLMANAGER_H
