#ifndef SETTINGS_H
#define SETTINGS_H

#include "CustomWidget.h"
#include "ToolManager.h"
#include "HotkeyManager.h"
#include <QSqlDatabase>
#include <QDir>

typedef QMap<QString, QKeySequence> HotkeyMap;
typedef QMap<int, QString> HotkeyIdMap;

class Settings : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    void setToolManagerList(ToolManagerList *toolManagerList);
    QString loadSetting(const QString& key, const QString& defaultValue = QString()) const;


signals:
    void appActiveChanged();

private:
    QWidget *mBasePage;
    QWidget *mAppPage;
    QWidget *mShortcutsPage;
    ToolManagerList *mToolManagerList;
    HotkeyManager *mHotkeyManager;
    HotkeyMap *mHotkeyMap;
    HotkeyIdMap *mHotkeyIdMap;
    QDir mdbDir;
    QString mdbName;
    QSqlDatabase mdb;

    void initBasePage();
    void initAppPage();
    void initShortcutsPage();

    void jumpTool(QString toolName);

    // 数据库相关操作
    void closeDatabase();
    bool initializeDatabase();
    bool saveSetting(const QString& key, const QString& value) const;
    template <typename T>
    void loadSettingsHandler(T* widget, const QString& defaultValue = QString());


private slots:
    void buttonClicked();
    void comboBoxChanged(const QString);
    void checkBoxChecked(bool);
    void switchButtonChanged(bool);
    void onHotkeyPressed(int);
    void keySequenceChanged(QKeySequence);
};

#endif // SETTINGS_H
