/**
 * @file MacStyleCheckBox.cpp
 * @brief Mac样式的复选框
 */

#include "MacStyleCheckBox.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "utils/Custom.h"

MacStyleCheckBox::MacStyleCheckBox(const QString &text, QWidget *parent)
    : QCheckBox(text, parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mAnimation = new QPropertyAnimation(this, "checkBoxAnimationValue", this);
    mAnimation->setDuration(ANIMATION_NORMAL);
    connect(this, &QCheckBox::toggled, this, &MacStyleCheckBox::onToggled);

    addDropShadowEffect(this);

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MacStyleCheckBox::onThemeChanged);
}

void MacStyleCheckBox::onThemeChanged()
{
    update();
}

void MacStyleCheckBox::setCheckBoxAnimationValue(qreal value)
{
    mCheckBoxAnimationValue = value;
    update();
}

void MacStyleCheckBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // 绘制复选框
    QRect checkboxRect(0, 0, 14, 14);
    checkboxRect.moveTop((height() - checkboxRect.height()) / 2);
    checkboxRect.moveLeft(rect().left() + 2);

    QColor uncheckedColor = QColor(COLOR_WHITE);
    QColor checkedColor = ThemeManager::instance().primaryColor();
    QColor borderColor = QColor(COLOR_BORDER_DARK);

    // 根据选中状态设置边框颜色
    painter.setPen(isChecked() ? checkedColor : borderColor);
    painter.setBrush(uncheckedColor);

    // 动画效果
    if (isChecked()) {
        qreal alpha = mCheckBoxAnimationValue;
        QColor animatedColor = checkedColor;
        animatedColor.setAlphaF(alpha);
        painter.setBrush(animatedColor);
    }

    painter.drawRoundedRect(checkboxRect, RADIUS_SMALL, RADIUS_SMALL);

    // 绘制勾选标记
    if (isChecked()) {
        QSvgRenderer renderer(QString(":/ico/check_white.svg"));
        QRect checkMarkRect = checkboxRect.adjusted(0, 0, 0, 0);
        renderer.render(&painter, checkMarkRect);
    }

    // 绘制文本
    QRect textRect = rect().adjusted(checkboxRect.height() + 7, 0, -3, 0);
    painter.setPen(QColor(COLOR_TEXT_PRIMARY));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
}

void MacStyleCheckBox::onToggled(bool checked)
{
    mAnimation->stop();
    mAnimation->setStartValue(checked ? 0.0 : 1.0);
    mAnimation->setEndValue(checked ? 1.0 : 0.0);
    mAnimation->start();
}
