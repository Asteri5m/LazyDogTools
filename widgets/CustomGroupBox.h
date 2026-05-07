#ifndef CUSTOMGROUPBOX_H
#define CUSTOMGROUPBOX_H

/**
 * @file CustomGroupBox.h
 * @brief 自定义GroupBox
 */

#include <QGroupBox>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QColor>

class ThemeManager;

class CustomGroupBox : public QGroupBox
{
    Q_OBJECT
    Q_PROPERTY(int mAcrylicAlpha READ acrylicAlpha WRITE setAcrylicAlpha)

public:
    explicit CustomGroupBox(const QString &title, QWidget *parent = nullptr);

    // 亚克力透明度设置
    int acrylicAlpha() const { return mAcrylicAlpha; }
    void setAcrylicAlpha(int alpha);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onAcrylicEnabledChanged(bool enabled);
    void onAcrylicOpacityChanged(int opacity);
    void onCardOpacityChanged(int opacity);

private:
    int mAcrylicAlpha = 120;
};

#endif // CUSTOMGROUPBOX_H
