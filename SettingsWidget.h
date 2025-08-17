#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

/**
 * @file SettingsWidget.h
 * @author Asteri5m
 * @date 2025-02-07 20:30:58
 * @brief Settings的交互窗口
 */

#include "UAC.h"
#include "CustomWidget.h"
#include "Settings.h"

class SettingsWidget : public ToolWidgetModel
{
    Q_OBJECT
public:
    explicit SettingsWidget(Settings* settings, QWidget *parent = nullptr);
    ~SettingsWidget();

signals:
    void toolActiveChanged();

private:
    QWidget *mBasePage;
    QWidget *mAppPage;
    QWidget *mHotkeyPage;
    QWidget *mAboutPage;
    Settings *mSettings;

    void initBasePage();
    void initAppPage();
    void initHotkeyPage();
    void initAboutPage();

    void jumpTool(QString toolName);
    template<typename T>
    void loadConfigHandler(T *widget);

    void onCheckUpdateClicked();
    void onUpdateAvailable(const QString &newVersion, const QString &changelog);
    void onNoUpdateAvailable();
    void showRichText(const QString &title, const QString &fileName);

private slots:
    void buttonClicked();
    void comboBoxChanged(const QString);
    void checkBoxChecked(bool);
    void switchButtonChanged(bool);
    void keySequenceChanged(QKeySequence);
};

#endif // SETTINGSWIDGET_H
