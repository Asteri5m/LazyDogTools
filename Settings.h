#ifndef SETTINGS_H
#define SETTINGS_H

#include "CustomWidget.h"

class Settings : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);

signals:

private:
    QWidget *page1;
    QWidget *page2;
    QWidget *page3;

private slots:
    void apply();
};

#endif // SETTINGS_H
