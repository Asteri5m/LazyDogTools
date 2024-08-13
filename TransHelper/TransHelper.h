#ifndef TRANSHELPER_H
#define TRANSHELPER_H

#include <QWidget>
#include "CustomWidget.h"

class TransHelper : public ToolWidgetModel
{
    Q_OBJECT
public:
    explicit TransHelper(QWidget *parent = nullptr);

signals:
};

#endif // TRANSHELPER_H
