#ifndef AUDIOHELPERMANAGER_H
#define AUDIOHELPERMANAGER_H

#include "ToolManager.h"
#include "CustomWidget.h"

class AudioHelperManager : public ToolManager
{
    Q_OBJECT
public:

    void initUI() override;
    void initialize() override;


signals:

};

#endif // AUDIOHELPERMANAGER_H
