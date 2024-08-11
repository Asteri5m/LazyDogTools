#ifndef SETTINGS_H
#define SETTINGS_H

#include "Tool.h"
#include "SettingsWidget.h"

class Settings : public Tool
{
    Q_OBJECT
public:
    // 重载
    void initUI() override;
    void initialize() override;

signals:

protected:


};

#endif // SETTINGS_H
