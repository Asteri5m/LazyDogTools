#ifndef AUDIOHELPERWEDGIT_H
#define AUDIOHELPERWEDGIT_H

#include "CustomWidget.h"

class AudioHelperWedgit : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit AudioHelperWedgit(QWidget *parent = nullptr);

signals:

private:
    QWidget *page1;
    QWidget *page2;
    QWidget *page3;
};

#endif // AUDIOHELPERWEDGIT_H
