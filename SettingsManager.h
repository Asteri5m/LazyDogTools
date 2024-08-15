#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "ToolManager.h"
#include "Settings.h"

class SettingsManager : public ToolManager
{
    Q_OBJECT
public:
    // 重载
    void initUI() override;
    void initialize() override;

signals:

protected:


};

#endif // SETTINGSMANAGER_H
