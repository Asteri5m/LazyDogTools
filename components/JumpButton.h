#ifndef JUMPBUTTON_H
#define JUMPBUTTON_H

/**
 * @file JumpButton.h
 * @brief 跳转按钮
 */

#include <QPushButton>
#include <QPainter>
#include <QRect>
#include <QIcon>
#include <QSize>
#include <QPoint>

class ThemeManager;

class JumpButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JumpButton(const QString &text, QWidget *parent = nullptr);

public slots:
    void onThemeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // JUMPBUTTON_H
