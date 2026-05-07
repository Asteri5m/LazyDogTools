/**
 * @file HintLabel.cpp
 * @brief 提示标签
 */

#include "HintLabel.h"
#include "managers/ThemeManager.h"
#include "utils/Custom.h"

HintLabel::HintLabel(const QString &text, const QString &hint, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text.at(0), parent, f)
    , mHint(hint)
{
    setFixedSize(16, 16);
    setAlignment(Qt::AlignCenter);
    setToolTip(mHint);

    addDropShadowEffect(this);

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &HintLabel::onThemeChanged);
}

void HintLabel::onThemeChanged()
{
    update();
}

void HintLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // 绘制圆形背景
    painter.setBrush(ThemeManager::instance().primaryColor());
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(1, 1, 14, 14);

    // 绘制文字
    painter.setPen(Qt::white);

    QFont font = painter.font();
    const qreal scale = 1.0 / painter.device()->devicePixelRatioF();
    font.setPixelSize(8 * 2 * scale);
    painter.setFont(font);

    painter.drawText(rect(), Qt::AlignCenter, this->text());
}

void HintLabel::mousePressEvent(QMouseEvent *event)
{
    QToolTip::showText(event->globalPosition().toPoint(), mHint, this);
}
