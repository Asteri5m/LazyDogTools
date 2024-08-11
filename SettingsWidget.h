#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "CustomWidget.h"

class SettingsWidget : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);

signals:

private:
    QWidget *page1;
    QWidget *page2;
    QWidget *page3;
};

#endif // SETTINGSWIDGET_H
