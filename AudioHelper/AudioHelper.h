#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#include "Tool.h"
#include "CustomWidget.h"

class AudioHelper : public Tool
{
    Q_OBJECT
public:

    void initUI() override;
    void initialize() override;


signals:

private:
};

#endif // AUDIOHELPER_H
