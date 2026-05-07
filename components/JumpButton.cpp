/**
 * @file JumpButton.cpp
 * @brief 跳转按钮
 */

#include "JumpButton.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "utils/Custom.h"

JumpButton::JumpButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    setFixedSize(48, 20);

    addDropShadowEffect(this);

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &JumpButton::onThemeChanged);
}

void JumpButton::onThemeChanged()
{
    update();
}

void JumpButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // 绘制背景
    QRect rect = this->rect();
    QColor backgroundColor = isDown() ? ThemeManager::instance().accentPressedColor() : ThemeManager::instance().primaryColor();
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, RADIUS_MEDIUM, RADIUS_MEDIUM);

    // 绘制图标
    QIcon icon(":/ico/jump_white.svg");
    QSize iconSize(20, 20);
    QPoint iconPos((width() - iconSize.width()) / 2, (height() - iconSize.height()) / 2);

    painter.drawPixmap(iconPos, icon.pixmap(iconSize));
}
