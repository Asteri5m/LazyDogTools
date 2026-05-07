/**
 * @file ToolManager.cpp
 * @author Asteri5m
 * @date 2025-02-07 18:09:32
 * @brief 工具管理器，单例
 */

#include "ToolManager.h"

ToolManager::ToolManager() {}

ToolManager& ToolManager::instance()
{
    static ToolManager instance;
    return instance;
}

ToolModel* ToolManager::createTool(const QString& toolID) 
{
    // 如果工具已经创建，则直接返回已有的实例
    if (mCreatedTools.contains(toolID)) 
        return mCreatedTools[toolID];

    // 否则，通过工厂函数创建新的工具实例
    if (mToolInfoMap.contains(toolID) && mToolInfoMap[toolID].enabled && mToolFactories.contains(toolID)) 
    {
        qDebug() << "初始化工具:" << toolID;
        ToolModel* tool = mToolFactories[toolID]();
        mCreatedTools[toolID] = tool;
        return tool;
    }

    // 如果找不到对应的工具，返回 nullptr
    return nullptr;
}

const ToolInfo &ToolManager::getToolInfo(const QString &toolID)
{
    static ToolInfo defaultToolInfo;  // 用于返回时的默认 ToolInfo
    return mToolInfoMap.contains(toolID) ? mToolInfoMap[toolID] : defaultToolInfo;
}

const ToolInfoMap &ToolManager::getAllTools() const 
{
    return mToolInfoMap;
}

ToolModel* ToolManager::getCreatedTool(const QString& toolID) const 
{
    // 返回已创建的工具实例，如果没有找到则返回 nullptr
    return mCreatedTools.value(toolID, nullptr);
}

void ToolManager::disableTool(const QString& toolID) 
{
    qInfo() << "禁用工具:" << toolID;
    // 检查是否存在该工具的实例
    if (mCreatedTools.contains(toolID)) 
    {
        delete mCreatedTools[toolID];  // 销毁工具实例
        mCreatedTools.remove(toolID);  // 从映射中移除
    }
    mToolInfoMap[toolID].enabled = false;
}

void ToolManager::enableTool(const QString &toolID)
{
    qInfo() << "启用工具:" << toolID;
    mToolInfoMap[toolID].enabled = true;
    createTool(toolID);
}
