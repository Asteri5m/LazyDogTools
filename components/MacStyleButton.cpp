/**
 * @file MacStyleButton.cpp
 * @brief Mac样式的按钮
 */

#include "MacStyleButton.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "utils/Custom.h"

MacStyleButton::MacStyleButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    setFixedHeight(25);
    setMinimumWidth(90);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // 去掉系统默认绘制
    setFlat(true);

    addDropShadowEffect(this);

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MacStyleButton::onThemeChanged);
}

void MacStyleButton::setNormalColorBlue(bool isBlue)
{
    mIsBlue = isBlue;
    update();
}

void MacStyleButton::onThemeChanged()
{
    update(); // 触发重绘以应用新主题颜色
}

void MacStyleButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRectF rect = this->rect();
    rect.adjust(1, 1, -1, -1); // 避免边缘模糊

    // 颜色定义
    QColor bgStart, bgEnd, borderColor, textColor;

    if (mIsBlue)
    {
        bgStart = ThemeManager::instance().primaryColor();
        bgEnd   = ThemeManager::instance().accentColor();
        borderColor = Qt::transparent;
        textColor = Qt::white;
    }
    else
    {
        bgStart = QColor(COLOR_WHITE);
        bgEnd   = QColor(COLOR_WHITE);
        borderColor = QColor(COLOR_BUTTON_BORDER);
        textColor = QColor(COLOR_TEXT_PRIMARY);
    }

    if (isDown())
    {
        bgStart = ThemeManager::instance().accentPressedColor();
        bgEnd   = ThemeManager::instance().accentPressedColor();
        borderColor = Qt::transparent;
        textColor = Qt::white;
    }

    // 绘制圆角背景
    QPainterPath path;
    path.addRoundedRect(rect, RADIUS_MEDIUM, RADIUS_MEDIUM);

    // 渐变
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0, bgStart);
    gradient.setColorAt(1, bgEnd);

    painter.fillPath(path, gradient);

    // 边框（1px 精准绘制）
    if (borderColor != Qt::transparent)
    {
        QPen pen(borderColor);
        pen.setWidthF(1.0);
        painter.setPen(pen);
        painter.drawPath(path);
    }

    // 文本
    painter.setPen(textColor);
    painter.setFont(font());

    painter.drawText(rect, Qt::AlignCenter, text());
}
