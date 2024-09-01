#ifndef SETTINGS_H
#define SETTINGS_H

#include "CustomWidget.h"
#include "ToolManager.h"
#include "HotkeyManager.h"

class Settings : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);

    void setToolManagerList(ToolManagerList *toolManagerList);

signals:
    void appActiveChanged();

private:
    QWidget *mBasePage;
    QWidget *mAppPage;
    QWidget *mShortcutsPage;
    ToolManagerList *mToolManagerList;
    HotkeyManager *mHotkeyManager;

    void initBasePage();
    void initAppPage();
    void initShortcutsPage();

    void jumpTool(QString toolName);

private slots:
    void buttonClicked();
    void comboBoxChanged(const QString);
    void checkBoxChecked(bool);
    void switchButtonChanged(bool);
    void onHotkeyPressed(int);
    void keySequenceChanged(QKeySequence);
};

#endif // SETTINGS_H
