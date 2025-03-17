/**
 * @file ToolModel.cpp
 * @author Asteri5m
 * @date 2025-02-07 17:28:58
 * @brief 工具的模版，建议所有的工具都继承此类
 */

#include "ToolModel.h"


ToolModel::ToolModel(QObject *parent)
    : QObject { parent }
    , mToolWidget (nullptr)
{

}

ToolModel::~ToolModel()
{
    if (mToolWidget != nullptr)
        mToolWidget->deleteLater();
}

TrayList *ToolModel::getTray()
{
    return mTray;
}

void ToolModel::setTray(TrayList *tray)
{
    mTray = tray;
}

void ToolModel::showWindow()
{
    if (mToolWidget == nullptr)
    {
        mToolWidget = new ToolWidgetModel();
        connect(mToolWidget, SIGNAL(closed()), this, SLOT(toolWindowClosed()));
        connect(mToolWidget, SIGNAL(windowEvent(QString,QString)), this, SLOT(toolWindowEvent(QString,QString)));
    }
    mToolWidget->show();
    mToolWidget->activateWindow();
}

void ToolModel::toolWindowClosed()
{
    mToolWidget = nullptr;
}

void ToolModel::toolWindowEvent(const QString &type, const QString &context)
{
    qDebug() << "未处理事件, type:" << type << ", context:" << context;
}

void ToolModel::hotKeyEvent(const QString &event)
{
    qDebug() << "未处理事件:" << event;
}
