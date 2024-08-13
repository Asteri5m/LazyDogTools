#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#include "CustomWidget.h"

class AudioHelper : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit AudioHelper(QWidget *parent = nullptr);

signals:

private:
    QWidget *page1;
    QWidget *page2;
    QWidget *page3;
};

#endif // AUDIOHELPER_H
