#ifndef TOOLWIDGETMODEL_H
#define TOOLWIDGETMODEL_H

/**
 * @file ToolWidgetModel.h
 * @brief 工具窗口模板，可以根据该模板快生成一个具有左侧菜单栏的"TabWidget"
 */

#include "AcrylicWidget.h"
#include "TransparentWidget.h"
#include "components/LeftMenuButton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QFrame>
#include <QVector>

class ThemeManager;

class ToolWidgetModel : public AcrylicWidget
{
    Q_OBJECT
public:
    ToolWidgetModel(QWidget *parent = nullptr);

private slots:
    void onAcrylicEnabledChanged(bool enabled);
    void onAcrylicOpacityChanged(int opacity);

public slots:
    void setDefaultStyle(bool menuRight = false);
    void addTab(QWidget* page, const QString &icon = nullptr, const QString &name = nullptr);
    void finalizeSetup();
    void showFirstPage();
    void onThemeChanged();

signals:
    void closed();
    void windowEvent(const QString &type, const QString &context);

protected:
    QHBoxLayout *mOverLayout;
    QGridLayout *mMainLayout;

    void closeEvent(QCloseEvent *event) override;

private:
    TransparentWidget *mMenuWidget;
    QVBoxLayout       *mMenuLayout;
    QStackedWidget    *mStackedWidget;
    QVector<LeftMenuButton*> mButtons;

    void resetButtonStates();
};

#endif // TOOLWIDGETMODEL_H
