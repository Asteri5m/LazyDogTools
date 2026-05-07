#ifndef MACSTYLEBUTTON_H
#define MACSTYLEBUTTON_H

/**
 * @file MacStyleButton.h
 * @brief Mac样式的按钮
 */

#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QRectF>
#include <QStyleOption>

class ThemeManager;

class MacStyleButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MacStyleButton(const QString &text, QWidget *parent = nullptr);

    void setNormalColorBlue(bool isBlue);

public slots:
    void onThemeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool mIsBlue { false };
};

#endif // MACSTYLEBUTTON_H
