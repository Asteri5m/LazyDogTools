#ifndef SMOOTHSCROLLAREA_H
#define SMOOTHSCROLLAREA_H

/**
 * @file SmoothScrollArea.h
 * @brief 平滑滚动区域
 */
#include <QScrollArea>
#include <QTimer>
#include <QWheelEvent>
#include <QScrollBar>

class SmoothScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit SmoothScrollArea(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onSmoothScroll();

private:
    QTimer *mScrollTimer;

    double mVelocity = 0.0;
    double mCurrentOffset = 0.0;

    // 参数（核心）
    const double mFriction = 0.92;      // 摩擦力
    const double mMinVelocity = 0.9;    // 停止阈值
    const double mWheelStep = 1.15;     // 滚轮增益
};

#endif // SMOOTHSCROLLAREA_H
