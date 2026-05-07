#ifndef TRANSPARENTWIDGET_H
#define TRANSPARENTWIDGET_H

/**
 * @file TransparentWidget.h
 * @brief 透明背景容器Widget - 用于配合亚克力效果
 */

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include "utils/Constants.h"

// class ThemeManager;

class TransparentWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal cornerRadius READ cornerRadius WRITE setCornerRadius)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
    Q_PROPERTY(QColor solidColor READ solidColor WRITE setSolidColor)
    Q_PROPERTY(QColor acrylicColor READ acrylicColor WRITE setAcrylicColor)
    Q_PROPERTY(int acrylicAlpha READ acrylicAlpha WRITE setAcrylicAlpha)

public:
    explicit TransparentWidget(QWidget *parent = nullptr);

    // 圆角设置
    qreal cornerRadius() const { return mCornerRadius; }
    void setCornerRadius(qreal radius);

    // 边框颜色设置
    QColor borderColor() const { return mBorderColor; }
    void setBorderColor(const QColor &color);

    // 边框宽度设置
    int borderWidth() const { return mBorderWidth; }
    void setBorderWidth(int width);

    // 默认颜色（亚克力关闭时使用）
    QColor solidColor() const { return mSolidColor; }
    void setSolidColor(const QColor &color);

    // 亚克力颜色（亚克力开启时使用）
    QColor acrylicColor() const { return mAcrylicColor; }
    void setAcrylicColor(const QColor &color);

    // 亚克力透明度设置
    int acrylicAlpha() const { return mAcrylicAlpha; }
    void setAcrylicAlpha(int alpha);

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onAcrylicEnabledChanged(bool enabled);
    void onAcrylicOpacityChanged(int opacity);

private:
    qreal mCornerRadius = 0;   // 圆角半径
    int mBorderWidth = 0;      // 边框宽度
    int mAcrylicAlpha = 0;     // 亚克力透明度 (0-255)
    QColor mSolidColor = QColor(COLOR_BG_WIDGET);  // 默认颜色（亚克力关闭时）
    QColor mBorderColor;                           // 边框颜色, 默认无
    QColor mAcrylicColor = QColor(COLOR_WHITE);    // 亚克力颜色（亚克力开启时，默认为白色）
};

#endif // TRANSPARENTWIDGET_H
