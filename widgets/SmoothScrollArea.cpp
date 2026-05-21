/**
 * @file SmoothScrollArea.cpp
 * @brief 平滑滚动区域
 */

#include "SmoothScrollArea.h"

SmoothScrollArea::SmoothScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);

    viewport()->setAttribute(Qt::WA_TranslucentBackground);

    setWidgetResizable(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 平滑滚动 Timer
    mScrollTimer = new QTimer(this);

    connect(mScrollTimer, &QTimer::timeout,
            this, &SmoothScrollArea::onSmoothScroll);

    mScrollTimer->setInterval(16); // 60 FPS

    QString scrollBarStyle = R"(
            /* 整个滚动条区域 */
            QScrollBar:vertical {
                background: transparent;   /* 背景透明 */
                width: 12px;               /* 控制粗细 */
                margin: 2px;               /* 留一点边距更好看 */
            }

            /* 滑块 */
            QScrollBar::handle:vertical {
                background: rgba(120, 120, 120, 160);
                border-radius: 4px;
                min-height: 30px;
            }

            /* hover */
            QScrollBar::handle:vertical:hover {
                background: rgba(120, 120, 120, 220);
            }

            /* 按下 */
            QScrollBar::handle:vertical:pressed {
                background: rgba(120, 120, 120, 255);
            }

            /* 上下按钮 */
            QScrollBar::sub-line:vertical,
            QScrollBar::add-line:vertical {
                height: 0px;
                background: none;
            }

            /* 空白区域 */
            QScrollBar::add-page:vertical,
            QScrollBar::sub-page:vertical {
                background: transparent;
            }

            QScrollBar:horizontal {
                background: transparent;
                height: 12px;
                margin: 2px;
            }

            QScrollBar::handle:horizontal {
                background: rgba(120, 120, 120, 160);
                border-radius: 4px;
                min-width: 30px;
            }

            QScrollBar::handle:horizontal:hover {
                background: rgba(120, 120, 120, 220);
            }

            QScrollBar::handle:horizontal:pressed {
                background: rgba(120, 120, 120, 255);
            }

            QScrollBar::sub-line:horizontal,
            QScrollBar::add-line:horizontal {
                width: 0px;
                background: none;
            }

            QScrollBar::add-page:horizontal,
            QScrollBar::sub-page:horizontal {
                background: transparent;
            }
            )";
        horizontalScrollBar()->setStyleSheet(scrollBarStyle);
        verticalScrollBar()->setStyleSheet(scrollBarStyle);
}

void SmoothScrollArea::wheelEvent(QWheelEvent *event)
{
    // 获取滚轮增量
    const double delta = event->angleDelta().y();

    // 累积速度（关键）
    mVelocity += (-delta / 120.0) * 8.0 * mWheelStep;

    // 限制最大速度（防止飞出去）
    mVelocity = qBound(-120.0, mVelocity, 120.0);

    // 启动惯性
    if (!mScrollTimer->isActive())
    {
        mScrollTimer->start();
    }

    event->accept();
}

void SmoothScrollArea::onSmoothScroll()
{
    // 速度衰减
    mVelocity *= mFriction;

    // 停止条件
    if (qAbs(mVelocity) < mMinVelocity)
    {
        mVelocity = 0;
        mScrollTimer->stop();
        return;
    }

    // 当前滚动位置
    QScrollBar *bar = verticalScrollBar();

    mCurrentOffset = bar->value();

    // 更新位置
    mCurrentOffset += mVelocity;

    // 像素对齐（防止 1px 抖动）
    int finalValue = qRound(mCurrentOffset);

    bar->setValue(finalValue);
}
