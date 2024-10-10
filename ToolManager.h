#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QKeySequence>
#include "CustomWidget.h"

typedef std::function<void()> Function;

struct HotKey {
    QString      Name;            // 快捷键名，会在设置中展示
    QKeySequence Shortkeys;       // 快捷键内容
    Function     Func;            // 回调函数
};

struct TrayItem {
    QString     Name;            // 条目名，在菜单中显示
    QString     Icon;            // 图标路径
    Function    Func;            // 回调函数
};

// 快捷键列表
typedef QList<HotKey> HotkeyList;
// 菜单栏条目，菜单名:关联函数
typedef QList<TrayItem> TrayList;

class ToolManager : public QObject {
    Q_OBJECT

public:
    explicit ToolManager(QObject *parent = nullptr);

    // 获取数据接口
    QString     getIcon();
    QString     getName();
    QString     getDescription();
    HotkeyList* getHotKey();
    TrayList*   getTray();
    bool        getActive();

    // 修改数据接口
    void setIcon(QString icon);
    void setName(QString name);
    void setDescription(QString desc);
    void setHotKey(HotkeyList* hotKeyList);
    void setTray(TrayList*  tray);
    void setActive(bool state);

    virtual void initUI();
    virtual void deleteUI();
    virtual void initialize();

public slots:
    virtual void show();
    virtual void hide();

private:
    QString     mIcon;          // 图标
    QString     mName;          // 应用名
    QString     mDescription;   // 应用描述
    HotkeyList* mHotKeyList;    // 快捷键列表
    TrayList*   mTray;          // 托盘菜单项
    bool        mActive;        // 工具启用状态

    void init();

protected:
    ToolWidgetModel* mToolWidget;

};


// 工具表
typedef QVector<ToolManager*> ToolManagerList;

#endif // TOOLMANAGER_H
