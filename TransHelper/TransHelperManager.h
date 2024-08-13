#ifndef TRANSHELPERMANAGER_H
#define TRANSHELPERMANAGER_H

#include <QObject>
#include "ToolManager.h"

class TransHelperManager : public ToolManager
{
    Q_OBJECT
public:
    void initUI() override;
    void initialize() override;
    // void show() override;

signals:

};

#endif // TRANSHELPERMANAGER_H
