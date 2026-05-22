/**
 * @file ToolWidgetModel.cpp
 * @brief 工具窗口模板实现
 */

#include "ToolWidgetModel.h"
#include "managers/ThemeManager.h"
#include "utils/Custom.h"
#include <QCloseEvent>

ToolWidgetModel::ToolWidgetModel(QWidget *parent)
    : AcrylicWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setAcrylicEnabled(ThemeManager::instance().acrylicEnabled());
    setAcrylicOpacity(ThemeManager::instance().acrylicOpacity());

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &ToolWidgetModel::onThemeChanged);
    connect(&ThemeManager::instance(), &ThemeManager::acrylicEnabledChanged, this, &ToolWidgetModel::onAcrylicEnabledChanged);
    connect(&ThemeManager::instance(), &ThemeManager::acrylicOpacityChanged, this, &ToolWidgetModel::onAcrylicOpacityChanged);
}

void ToolWidgetModel::onAcrylicEnabledChanged(bool enabled)
{
    setAcrylicEnabled(enabled);
}

void ToolWidgetModel::onAcrylicOpacityChanged(int opacity)
{
    setAcrylicOpacity(opacity);
}

void ToolWidgetModel::setDefaultStyle(bool menuRight)
{
    // 全局背景，渲染整体背景用，否则会黑屏
    QVBoxLayout *backLayout = new QVBoxLayout(this);
    TransparentWidget *backWidget = new TransparentWidget();
    backLayout->setContentsMargins(0, 0, 0, 0);
    backLayout->addWidget(backWidget);

    mOverLayout = new QHBoxLayout(backWidget);
    mMainLayout = new QGridLayout();
    mOverLayout->setSpacing(PADDING_MEDIUM);
    mOverLayout->setContentsMargins(MARGIN_MEDIUM, MARGIN_TINY, MARGIN_MEDIUM , MARGIN_MEDIUM);
    mMainLayout->setContentsMargins(0, 0, 0, 0);

    // 菜单栏背景
    mMenuWidget = new TransparentWidget();
    mMenuWidget->setFixedWidth(72);
    mMenuWidget->setCornerRadius(RADIUS_LARGE);
    mMenuWidget->setBorderWidth(1);
    mMenuWidget->setBorderColor(QColor(COLOR_BORDER));
    mMenuWidget->setSolidColor(QColor(COLOR_BG_CARD));
    mMenuWidget->setAcrylicColor(QColor(COLOR_BG_CARD));
    mMenuWidget->setAcrylicAlpha(ThemeManager::instance().cardOpacity() * 255 / 100);
    connect(&ThemeManager::instance(), &ThemeManager::cardOpacityChanged, mMenuWidget, &TransparentWidget::onAcrylicOpacityChanged);

    // 添加阴影
    addDropShadowEffect(mMenuWidget);

    mMenuLayout = new QVBoxLayout(mMenuWidget);
    mMenuLayout->addStretch();
    mMenuLayout->setContentsMargins(0, MARGIN_LARGE, 0, MARGIN_LARGE);
    mMenuLayout->setAlignment(Qt::AlignCenter);
    mMenuLayout->setSpacing(15);

    mStackedWidget = new QStackedWidget();
    mStackedWidget->setAttribute(Qt::WA_TranslucentBackground);
    mMainLayout->addWidget(mStackedWidget);

    if (menuRight)
    {
        mOverLayout->addLayout(mMainLayout);
        mOverLayout->addWidget(mMenuWidget); 
    }
    else
    {
        mOverLayout->addWidget(mMenuWidget);
        mOverLayout->addLayout(mMainLayout);
    }

    QString style = QString(
        "QPushButton, QPushButton * {"
        "    border: none;"
        "}"
        "QPushButton:hover {"
        "    background-color: %1; "
        "    border-radius: %4px;"
        "}"
        "QPushButton:checked {"
        "    background-color: %2;"
        "    border-radius: %4px;"
        "}"
        "QPushButton:checked:hover {"
        "    background-color: %3;"
        "    border-radius: %4px;"
        "}")
        .arg(ThemeManager::instance().primaryHoverColor().name())
        .arg(ThemeManager::instance().primaryColor().name())
        .arg(ThemeManager::instance().primaryPressedHoverColor().name())
        .arg(RADIUS_LARGE);

    mMenuWidget->setStyleSheet(style);
}

void ToolWidgetModel::addTab(QWidget* page, const QString &icon, const QString &name)
{
    LeftMenuButton *button = new LeftMenuButton(icon, name);
    button->setStyleSheet("border: none;");
    mMenuLayout->insertWidget(mMenuLayout->count() - 1, button);
    mButtons.append(button);

    mStackedWidget->addWidget(page);

    connect(button, &LeftMenuButton::clicked, this, [=]() {
        resetButtonStates();
        mStackedWidget->setCurrentWidget(page);
        button->setChecked(true);
    });
}

void ToolWidgetModel::finalizeSetup()
{
    if (!mButtons.isEmpty())
    {
        mButtons.first()->click();
    }
}

void ToolWidgetModel::showFirstPage()
{
    if (!mButtons.isEmpty())
        mButtons.first()->click();
    show();
}

void ToolWidgetModel::onThemeChanged()
{
    QString style = QString(
        "QPushButton, QPushButton * {"
        "    border: none;"
        "}"
        "QPushButton:hover {"
        "    background-color: %1; "
        "    border-radius: %4px;"
        "}"
        "QPushButton:checked {"
        "    background-color: %2;"
        "    border-radius: %4px;"
        "}"
        "QPushButton:checked:hover {"
        "    background-color: %3;"
        "    border-radius: %4px;"
        "}")
        .arg(ThemeManager::instance().primaryHoverColor().name())
        .arg(ThemeManager::instance().primaryColor().name())
        .arg(ThemeManager::instance().primaryPressedHoverColor().name())
        .arg(RADIUS_LARGE);

    mMenuWidget->setStyleSheet(style);
    mMenuWidget->update();
}

void ToolWidgetModel::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

void ToolWidgetModel::resetButtonStates()
{
    for (auto button = mButtons.constBegin(); button != mButtons.constEnd(); button++)
    {
        (*button)->setChecked(false);
    }
}
