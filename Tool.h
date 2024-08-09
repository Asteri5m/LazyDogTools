#ifndef TOOL_H
#define TOOL_H

#include <QWidget>
#include <QShortcut>

struct ShortCut {
    QString    Name;            // 快捷键名，会在设置中展示
    QShortcut* Shortkeys;       // 快捷键内容
    void*      Func;            // 需要与快捷键关联的函数
};

// 快捷键列表
typedef QList<ShortCut>      ShortList;
// 菜单栏条目，菜单名:关联函数
typedef QMap<QString, void*> TrayList;


// 基类模板
template <typename T>
class Tool : public QWidget
{
public:
    explicit Tool(QWidget *parent = nullptr);

    // 获取静态数据接口
    static QString     GetIcon()        { return mIcon; }
    static QString     GetName()        { return mName; }
    static QString     GetDescription() { return mDescription; }
    static ShortList   GetShortcut()    { return mShortcut; }
    static TrayList    GetTrayn()       { return mTray; }
    static bool        GetActive()      { return mActive; }

    // 修改启用状态
    static void SetActive(bool state)   { mActive = state; }

    // 用于初始化参数
    static void initialize(
        QString   icon,
        QString   name,
        QString   description,
        ShortList shortcut,
        TrayList  tray ) {

        mIcon = icon;
        mName = name;
        mDescription = description;
        mShortcut = shortcut;
        mTray = tray;
    }


signals:


protected:
    static QString   mIcon;          // 图标
    static QString   mName;          // 应用名
    static QString   mDescription;   // 应用描述
    static ShortList mShortcut;      // 快捷键列表
    static TrayList  mTray;          // 托盘菜单项
    static bool      mActive;        // 工具启用状态
};

// 静态成员变量的定义
template <typename T>
QString Tool<T>::mIcon = "";

template <typename T>
QString Tool<T>::mName = "";

template <typename T>
QString Tool<T>::mDescription = "";

template <typename T>
ShortList Tool<T>::mShortcut = nullptr;

template <typename T>
TrayList Tool<T>::mTray = nullptr;

template <typename T>
bool Tool<T>::mActive = true;

#endif // TOOL_H
