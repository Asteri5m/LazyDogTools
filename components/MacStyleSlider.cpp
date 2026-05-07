/**
 * @file MacStyleSlider.cpp
 * @brief Mac样式滑块
 */

#include "MacStyleSlider.h"
#include "managers/ThemeManager.h"
#include "utils/Custom.h"
#include "utils/Constants.h"

MacStyleSlider::MacStyleSlider(QWidget *parent)
    : QSlider(parent)
{
    setFixedHeight(20);
    setOrientation(Qt::Horizontal);

    addDropShadowEffect(this);

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MacStyleSlider::onThemeChanged);
}

void MacStyleSlider::onThemeChanged()
{
    update();
}

void MacStyleSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    int handleRadius = RADIUS_LARGE;
    int handleMargin = handleRadius + 4;

    QRect grooveRect = rect().adjusted(handleMargin, 0, -handleMargin, 0);
    int grooveHeight = 4;
    int grooveY = (height() - grooveHeight) / 2;
    grooveRect.setTop(grooveY);
    grooveRect.setHeight(grooveHeight);

    // 轨道背景
    QPainterPath groovePath;
    groovePath.addRoundedRect(grooveRect, grooveHeight / 2, grooveHeight / 2);
    p.fillPath(groovePath, QColor(COLOR_SWITCH_OFF));

    // 已填充部分
    double ratio = double(value() - minimum()) / (maximum() - minimum());
    int fillWidth = int(grooveRect.width() * ratio);
    QRect fillRect = grooveRect;
    fillRect.setLeft(grooveRect.left());
    fillRect.setWidth(fillWidth);

    QPainterPath fillPath;
    fillPath.addRoundedRect(fillRect, grooveHeight / 2, grooveHeight / 2);
    p.fillPath(fillPath, ThemeManager::instance().primaryColor());

    // 滑块
    int handleX = grooveRect.left() + fillWidth;
    int handleY = height() / 2;
    QRect handleRect(handleX - handleRadius, handleY - handleRadius, handleRadius * 2, handleRadius * 2);

    p.setBrush(Qt::white);
    p.drawEllipse(handleRect);

    QPen pen(QColor(COLOR_TRACK));
    pen.setWidthF(0.5);
    p.setPen(pen);
    p.drawEllipse(handleRect.adjusted(0, 0, 0, 0));
}

void MacStyleSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    update();
}

void MacStyleSlider::mouseMoveEvent(QMouseEvent *event)
{
    QSlider::mouseMoveEvent(event);
    update();
}

void MacStyleSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);
    update();
}
