#ifndef SETTINGS_H
#define SETTINGS_H

#include "CustomWidget.h"
#include "ToolManager.h"

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

    void initBasePage();
    void initAppPage();
    void initShortcutsPage();

private slots:
    void buttonClicked();
    void comboBoxChanged(const QString);
    void checkBoxChecked(bool);
};

#endif // SETTINGS_H
