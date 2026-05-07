#ifndef MACSTYLESLIDER_H
#define MACSTYLESLIDER_H

/**
 * @file MacStyleSlider.h
 * @brief Mac样式ComboBox---Mac样式滑块
 */

#include <QSlider>
#include <QPainter>
#include <QRect>
#include <QPainterPath>
#include <QEnterEvent>
#include <QMouseEvent>

class ThemeManager;

class MacStyleSlider : public QSlider
{
    Q_OBJECT
public:
    explicit MacStyleSlider(QWidget *parent = nullptr);

public slots:
    void onThemeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // MACSTYLESLIDER_H
