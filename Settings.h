#ifndef SETTINGS_H
#define SETTINGS_H

#include "CustomWidget.h"

class Settings : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);

signals:

private:
    QWidget *mBasePage;
    QWidget *mAppPage;
    QWidget *mShortcutsPage;

    void initBasePage();
    void initAppPage();
    void initShortcutsPage();

private slots:
    void apply();
};

#endif // SETTINGS_H
