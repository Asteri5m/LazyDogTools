#ifndef TRANSHELPERMANAGER_H
#define TRANSHELPERMANAGER_H

#include <QObject>
#include "Tool.h"

class TransHelperManager : public Tool
{
    Q_OBJECT
public:
    void initUI() override;
    void initialize() override;
    // void show() override;

signals:

};

#endif // TRANSHELPERMANAGER_H
