/**
 * @file MacSwitchButton.cpp
 * @brief Mac样式开关
 */

#include "MacSwitchButton.h"
#include "managers/ThemeManager.h"
#include "utils/Custom.h"
#include "utils/Constants.h"

MacSwitchButton::MacSwitchButton(const QString &text, QWidget *parent)
    : QWidget(parent), mText(text)
{
    setFixedSize(40, 20);
    // 切换动画
    mAnimation = new QPropertyAnimation(this, "offset", this);
    mAnimation->setDuration(200);
    mAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    // 鼠标悬浮动画
    mHoverAnimation = new QPropertyAnimation(this, "thumbScale", this);
    mHoverAnimation->setDuration(150);
    mHoverAnimation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(mAnimation, &QPropertyAnimation::finished, this, [=](){
        mIsAnimating = false;
        emit checkedChanged(mChecked);
    });

    addDropShadowEffect(this);

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MacSwitchButton::onThemeChanged);
}

void MacSwitchButton::onThemeChanged()
{
    update(); // 触发重绘以应用新主题颜色
}

void MacSwitchButton::setOffset(qreal offset)
{
    mOffset = offset;
    emit offsetChanged();
    update();
}

void MacSwitchButton::setChecked(bool checked)
{
    if (mChecked == checked) return;
    mChecked = checked;
    mOffset = checked ? 1.0 : 0.0;
    update();
}

void MacSwitchButton::setThumbScale(qreal scale)
{
    mThumbScale = scale;
    emit thumbScaleChanged();
    update();
}

void MacSwitchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QColor bgColor = QColor(mChecked ? ThemeManager::instance().primaryColor().name() : COLOR_SWITCH_OFF);
    QColor thumbColor = QColor(COLOR_THUMB);
    QColor borderColor = QColor(mChecked ? ThemeManager::instance().primaryColor().name() : COLOR_SWITCH_OFF);

    // 绘制背景
    painter.setPen(QPen(borderColor, 1.5));
    painter.setBrush(bgColor);
    painter.drawRoundedRect(mButtonRect, mRadius, mRadius);

    // 绘制滑块
    painter.setPen(Qt::NoPen);
    qreal thumbWidth = height() * mThumbScale;
    qreal thumbHeight = height() * mThumbScale;
    qreal thumbX = mOffset * (width() - height()) + (height() - thumbWidth) / 2;
    qreal thumbY = (height() - thumbHeight) / 2;
    QRectF thumbRect(thumbX, thumbY, thumbWidth, thumbHeight);
    painter.setBrush(thumbColor);
    painter.drawEllipse(thumbRect);
}

void MacSwitchButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !mIsAnimating) {
        toggleChecked();
    }
}

void MacSwitchButton::resizeEvent(QResizeEvent *event)
{
    int height = this->height();
    int width = this->width();

    // 确保圆角的半径和高度相匹配
    mRadius = (height) / 2;

    // 更新开关按钮的位置与大小
    int padding = 1;
    mButtonRect = QRect(padding, padding, width - padding * 2, height - padding * 2);

    QWidget::resizeEvent(event);
}

void MacSwitchButton::enterEvent(QEnterEvent *event)
{
    mHoverAnimation->stop();
    mHoverAnimation->setStartValue(mThumbScale);
    mHoverAnimation->setEndValue(0.6);
    mHoverAnimation->start();
    QWidget::enterEvent(event);
}

void MacSwitchButton::leaveEvent(QEvent *event)
{
    mHoverAnimation->stop();
    mHoverAnimation->setStartValue(mThumbScale);
    mHoverAnimation->setEndValue(0.75);
    mHoverAnimation->start();
    QWidget::leaveEvent(event);
}

void MacSwitchButton::toggleChecked()
{
    if (mIsAnimating) return;
    mIsAnimating = true;

    mChecked = !mChecked;
    mAnimation->setStartValue(mChecked ? 0 : 1);
    mAnimation->setEndValue(mChecked ? 1 : 0);
    mAnimation->start();
}
