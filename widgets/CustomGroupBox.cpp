/**
 * @file CustomGroupBox.cpp
 * @brief 自定义GroupBox
 */

#include "CustomGroupBox.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "utils/Custom.h"

CustomGroupBox::CustomGroupBox(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
{
    setContentsMargins(12, 24, 12, 0); // 内容 padding

    // 添加阴影
    addDropShadowEffect(this);

    // 连接亚克力效果变化信号
    connect(&ThemeManager::instance(), &ThemeManager::acrylicEnabledChanged, this, &CustomGroupBox::onAcrylicEnabledChanged);
    connect(&ThemeManager::instance(), &ThemeManager::acrylicOpacityChanged, this, &CustomGroupBox::onAcrylicOpacityChanged);
    connect(&ThemeManager::instance(), &ThemeManager::cardOpacityChanged,    this, &CustomGroupBox::onCardOpacityChanged);
}

void CustomGroupBox::setAcrylicAlpha(int alpha)
{
    mAcrylicAlpha = alpha * 255 / 100;
    update();
}

void CustomGroupBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRectF rect = this->rect();
    rect.adjust(0.5, 0.5, -0.5, -0.5); // 防止边缘虚化

    const qreal radius = RADIUS_XLARGE;

    // 根据亚克力状态和卡片不透明度选择背景色
    QColor bgColor = QColor(COLOR_BG_CARD);
    if (ThemeManager::instance().acrylicEnabled()) {
        bgColor.setAlpha(qBound(0, mAcrylicAlpha, 255));
    }

    // 绘制白色圆角背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect, radius, radius);
    painter.fillPath(bgPath, bgColor);

    // 边框
    QPen borderPen(QColor(COLOR_BORDER));
    borderPen.setWidthF(1.0);
    painter.setPen(borderPen);
    painter.drawPath(bgPath);

    //绘制标题
    QFont titleFont = font();
    titleFont.setWeight(QFont::Medium);
    titleFont.setPointSize(FONT_SIZE_MEDIUM);
    painter.setFont(titleFont);

    painter.setPen(QColor(COLOR_TEXT_PRIMARY));

    QRect titleRect(12, 8, width() - 24, 20);
    painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, title());
}

void CustomGroupBox::onAcrylicEnabledChanged(bool enabled)
{
    Q_UNUSED(enabled);
    update();  // 亚克力状态变化时重绘
}

void CustomGroupBox::onAcrylicOpacityChanged(int opacity)
{
    Q_UNUSED(opacity);
    update();  // 亚克力透明度变化时重绘
}

void CustomGroupBox::onCardOpacityChanged(int opacity)
{
    setAcrylicAlpha(opacity);
    update();  // 卡片不透明度变化时重绘
}
