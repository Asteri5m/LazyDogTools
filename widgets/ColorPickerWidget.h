#ifndef COLORPICKERWIDGET_H
#define COLORPICKERWIDGET_H

/**
 * @file ColorPickerWidget.h
 * @brief 颜色选择器控件
 */

#include <QWidget>
#include <QPushButton>
#include <QColor>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QColorDialog>
#include <QDialog>
#include <QEnterEvent>
#include <QVector>
#include <QStyleOption>

// =====================================================
// ColorPickerButton - 颜色选择按钮
// =====================================================

class ColorPickerButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit ColorPickerButton(const QColor &color, QWidget *parent = nullptr);

    QColor color() const { return mColor; }
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor mColor;
};

// =====================================================
// ColorPickerPopup - 颜色选择弹出框
// =====================================================

class MacStyleButton;

class ColorPickerPopup : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPickerPopup(QWidget *parent = nullptr);
    ~ColorPickerPopup();

    QColor currentColor() const { return mCurrentColor; }
    void setCurrentColor(const QColor &color);

signals:
    void colorSelected(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupColorGrid();
    void setupCustomColor();
    void applyColor(const QColor &color);

private:
    QColor mCurrentColor;
    QVector<QColor> mPresetColors;
    QGridLayout *mColorGridLayout;
    QWidget *mCustomColorWidget;
    QLineEdit *mHexInput;
    QSlider *mOpacitySlider;
    QLabel *mPreviewLabel;
};

// =====================================================
// ColorPickerWidget - 完整颜色选择器
// =====================================================

class ColorPickerWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit ColorPickerWidget(QWidget *parent = nullptr);
    explicit ColorPickerWidget(const QColor &initialColor, QWidget *parent = nullptr);

    QColor color() const { return mColor; }
    void setColor(const QColor &color);

    QString text() const { return mTextLabel ? mTextLabel->text() : QString(); }
    void setText(const QString &text);

signals:
    void colorChanged(const QColor &color);

private slots:
    void onButtonClicked();
    void onColorSelected(const QColor &color);

private:
    void init();

private:
    QColor mColor;
    ColorPickerButton *mColorButton;
    QLabel *mTextLabel;
    ColorPickerPopup *mPopup;
    QLabel *mPreviewLabel;
};

#endif // COLORPICKERWIDGET_H
