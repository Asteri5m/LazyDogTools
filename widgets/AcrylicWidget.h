/**
 * @file AcrylicWidget.h
 * @author Asteri5m
 * @date 2025-05-05
 * @brief 亚克力窗口支持类
 */

#ifndef ACRYLICWIDGET_H
#define ACRYLICWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QColor>

#include <windows.h>

class AcrylicWidget : public QWidget
{
    Q_OBJECT

    // ==================== Q_PROPERTY 声明 ====================
    Q_PROPERTY(bool acrylicEnabled READ acrylicEnabled WRITE setAcrylicEnabled NOTIFY acrylicEnabledChanged)
    Q_PROPERTY(int acrylicOpacity READ acrylicOpacity WRITE setAcrylicOpacity NOTIFY acrylicOpacityChanged)
    Q_PROPERTY(QColor tintColor READ tintColor WRITE setTintColor NOTIFY tintColorChanged)

public:
    explicit AcrylicWidget(QWidget *parent = nullptr);
    ~AcrylicWidget() = default;

    // ==================== 启用状态 ====================
    /** 设置亚克力效果启用状态 */
    void setAcrylicEnabled(bool enabled);
    /** 获取亚克力效果启用状态 */
    bool acrylicEnabled() const;

    // ==================== 透明度控制 ====================
    /** 设置亚克力透明度 (0-100) */
    void setAcrylicOpacity(int opacity);
    /** 获取亚克力透明度 (0-100) */
    int acrylicOpacity() const;

    // ==================== 着色颜色 ====================
    /** 设置着色颜色 */
    void setTintColor(const QColor &color);
    /** 获取着色颜色 */
    QColor tintColor() const;

    // ==================== 辅助方法 ====================
    /** 获取是否支持系统级亚克力 */
    bool isSystemAcrylicSupported() const;

signals:
    /** 亚克力启用状态改变信号 */
    void acrylicEnabledChanged(bool enabled);
    /** 透明度改变信号 */
    void acrylicOpacityChanged(int opacity);
    /** 着色颜色改变信号 */
    void tintColorChanged(const QColor &color);

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void enableAcrylic();
    void disableAcrylic();
    void updateAcrylic();

private:
    bool     mEnabled = true;
    int      mOpacity = 0;           ///< 透明度 (0-100)
    int      mBlurRadius = 10;        ///< 模糊半径
    QColor   mTintColor = QColor(255, 255, 255, 255);  ///< 着色颜色

    using DwmSetWindowAttributeFunc = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
    DwmSetWindowAttributeFunc mDwmSetWindowAttribute = nullptr;
};

#endif // ACRYLICWIDGET_H
