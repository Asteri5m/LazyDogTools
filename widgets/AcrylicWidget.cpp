/**
 * @file AcrylicWidget.cpp
 * @author Asteri5m
 * @date 2026-05-05
 * @brief 亚克力窗口实现 - 完整优化版本
 */

#include "AcrylicWidget.h"
#include "managers/ThemeManager.h"
#include <QPainter>
#include <QDebug>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define DWMWA_SYSTEMBACKDROP_TYPE 38
#define DWMSBT_AUTO 2
#define DWMSBT_TRANSIENTWINDOW 3

AcrylicWidget::AcrylicWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    setAcrylicEnabled(ThemeManager::instance().acrylicEnabled());
    setAcrylicOpacity(ThemeManager::instance().acrylicOpacity());

    HMODULE lib = LoadLibraryW(L"dwmapi.dll");
    if (lib) {
        mDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFunc>(GetProcAddress(lib, "DwmSetWindowAttribute"));
    }
}

// ==================== 启用状态 ====================

void AcrylicWidget::setAcrylicEnabled(bool enabled)
{
    if (mEnabled == enabled)
        return;

    mEnabled = enabled;

    if (enabled)
        enableAcrylic();
    else
        disableAcrylic();

    update();
    emit acrylicEnabledChanged(mEnabled);
}

bool AcrylicWidget::acrylicEnabled() const
{
    return mEnabled;
}

// ==================== 透明度控制 ====================

void AcrylicWidget::setAcrylicOpacity(int opacity)
{
    int newOpacity = qBound(0, opacity, 100);
    if (mOpacity == newOpacity)
        return;

    mOpacity = newOpacity;
    update();
    emit acrylicOpacityChanged(mOpacity);
}

int AcrylicWidget::acrylicOpacity() const
{
    return mOpacity;
}

// ==================== 着色颜色 ====================

void AcrylicWidget::setTintColor(const QColor &color)
{
    if (mTintColor == color)
        return;

    mTintColor = color;
    update();
    emit tintColorChanged(mTintColor);
}

QColor AcrylicWidget::tintColor() const
{
    return mTintColor;
}

// ==================== 辅助方法 ====================

bool AcrylicWidget::isSystemAcrylicSupported() const
{
    return mDwmSetWindowAttribute != nullptr;
}

// ==================== 事件处理 ====================

void AcrylicWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // if (mEnabled)
    // {
        winId();

        QTimer::singleShot(0, this, [this]() {
            if (mEnabled)
                enableAcrylic();
            else
                disableAcrylic();
        });
    // }
}

void AcrylicWidget::updateAcrylic()
{
    if (!mEnabled)
        return;

    enableAcrylic();
    update();
}

void AcrylicWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor tint = mTintColor;
    int alpha = 0;

    if (mEnabled){
        // 将透明度 (0-100) 转换为 alpha (0-255)
        alpha = (mOpacity * 255) / 100;
    }

    tint.setAlpha(qBound(0, alpha, 255));

    p.fillRect(rect(), tint);
}

void AcrylicWidget::enableAcrylic()
{
    if (!mDwmSetWindowAttribute)
        return;

    HWND hwnd = (HWND)winId();
    if (!hwnd)
        return;

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    DWORD backdrop = DWMSBT_TRANSIENTWINDOW;
    HRESULT hr = mDwmSetWindowAttribute(
        hwnd,
        DWMWA_SYSTEMBACKDROP_TYPE,
        &backdrop,
        sizeof(backdrop)
        );

    qDebug() << "[Acrylic]" << (SUCCEEDED(hr) ? "OK" : "Fail");
}

void AcrylicWidget::disableAcrylic()
{
    if (!mDwmSetWindowAttribute)
        return;

    HWND hwnd = (HWND)winId();
    if (!hwnd)
        return;

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    DWORD backdrop = DWMSBT_AUTO;
    mDwmSetWindowAttribute(
        hwnd,
        DWMWA_SYSTEMBACKDROP_TYPE,
        &backdrop,
        sizeof(backdrop)
    );
}
