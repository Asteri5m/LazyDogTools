#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

/**
 * @file ToolManager.h
 * @author Asteri5m
 * @date 2025-02-07 18:09:32
 * @brief 工具管理器，单例
 */

#include "ToolModel.h"
#include <QMap>
#include <QObject>


struct ToolInfo {
    QString Name;
    QString IconPath;
    QString Description;
    QStringList HotkeyList;
    bool enabled;
};

template <typename ToolType>
using ToolFactory = std::function<ToolType*()>;

typedef QMap<QString, ToolInfo> ToolInfoMap;
typedef QMap<QString, std::function<ToolModel*()>> ToolFactories;
typedef QMap<QString, ToolModel*> CreatedToolsMap;  // 已创建的工具映射

class ToolManager
{
public:
    ToolManager();
    static ToolManager& instance();

    // 创建工具
    ToolModel* createTool(const QString& toolID);

    // 获取工具信息
    const ToolInfoMap &getAllTools() const;
    const ToolInfo &getToolInfo(const QString &toolID);

    // 获取已创建的工具
    ToolModel* getCreatedTool(const QString& toolID) const;

    // 禁用工具（销毁工具实例）
    void disableTool(const QString& toolID);

    // 启用工具
    void enableTool(const QString& toolID);

    // 注册工具
    template <typename ToolType>
    void registerTool(const QString& toolID, const ToolInfo& info, ToolFactory<ToolType> factoryMethod)
    {
        mToolInfoMap[toolID] = info;
        mToolFactories[toolID] = [factoryMethod]() -> ToolModel* { return factoryMethod(); };
    }

private:
    ToolInfoMap mToolInfoMap;  // 工具信息
    ToolFactories mToolFactories;  // 工厂函数
    CreatedToolsMap mCreatedTools;  // 已创建的工具对象
};

#endif // TOOLMANAGER_H
