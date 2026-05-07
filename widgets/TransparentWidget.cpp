/**
 * @file TransparentWidget.cpp
 * @brief 透明背景容器Widget - 用于配合亚克力效果
 */

#include "TransparentWidget.h"
#include "managers/ThemeManager.h"

TransparentWidget::TransparentWidget(QWidget *parent)
    : QWidget(parent)
{
    // 设置透明属性
    setAttribute(Qt::WA_TranslucentBackground);

    // 连接亚克力效果变化信号
    connect(&ThemeManager::instance(), &ThemeManager::acrylicEnabledChanged, this, &TransparentWidget::onAcrylicEnabledChanged);
}

void TransparentWidget::setCornerRadius(qreal radius)
{
    mCornerRadius = radius;
    update();
}

void TransparentWidget::setBorderColor(const QColor &color)
{
    mBorderColor = color;
    update();
}

void TransparentWidget::setBorderWidth(int width)
{
    mBorderWidth = width;
    update();
}

void TransparentWidget::setSolidColor(const QColor &color)
{
    mSolidColor = color;
    update();
}

void TransparentWidget::setAcrylicColor(const QColor &color)
{
    mAcrylicColor = color;
    update();
}

void TransparentWidget::setAcrylicAlpha(int alpha)
{
    mAcrylicAlpha = qBound(0, alpha, 255);
    update();
}

void TransparentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRectF rect = this->rect();
    if (mCornerRadius > 0) {
        rect.adjust(0.5, 0.5, -0.5, -0.5);
    }

    // 根据亚克力状态和卡片不透明度选择背景色
    QColor bgColor = mSolidColor;
    if (ThemeManager::instance().acrylicEnabled()) {
        bgColor = mAcrylicColor;
        bgColor.setAlpha(qBound(0, mAcrylicAlpha, 255));
    }

    // 绘制圆角矩形背景
    QPainterPath path;
    if (mCornerRadius > 0) {
        path.addRoundedRect(rect, mCornerRadius, mCornerRadius);
    } else {
        path.addRect(rect);
    }
    painter.fillPath(path, bgColor);

    // 绘制边框
    if (mBorderWidth > 0 && mBorderColor.isValid()) {
        QPen pen(mBorderColor);
        pen.setWidthF(mBorderWidth);
        painter.setPen(pen);
        painter.drawPath(path);
    }
}

void TransparentWidget::onAcrylicEnabledChanged(bool enabled)
{
    Q_UNUSED(enabled);
    update();
}

void TransparentWidget::onAcrylicOpacityChanged(int opacity)
{
    mAcrylicAlpha = opacity * 255 / 100;
    update();
}
