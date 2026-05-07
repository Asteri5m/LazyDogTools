#ifndef MACSWITCHBUTTON_H
#define MACSWITCHBUTTON_H

/**
 * @file MacSwitchButton.h
 * @brief Mac样式开关
 */

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QRect>
#include <QRectF>
#include <QColor>
#include <QEnterEvent>
#include <QEvent>

class ThemeManager;

class MacSwitchButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(qreal thumbScale READ thumbScale WRITE setThumbScale NOTIFY thumbScaleChanged)

public:
    explicit MacSwitchButton(const QString &text = nullptr, QWidget *parent = nullptr);

public slots:
    void onThemeChanged();

    qreal offset() const { return mOffset; }
    void setOffset(qreal offset);

    bool isChecked() const { return mChecked; }
    void setChecked(bool checked);

    QString text() { return mText; }

    qreal thumbScale() const { return mThumbScale; }
    void setThumbScale(qreal scale);

signals:
    void checkedChanged(bool checked);
    void offsetChanged();
    void thumbScaleChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void toggleChecked();

private:
    int mRadius { 10 };
    bool mIsAnimating { false };
    bool mChecked { false };
    bool mHovered { false };
    QRect mButtonRect { 0, 0, 0, 0 };
    qreal mOffset { 0 };
    qreal mThumbScale { 0.75 };
    QString mText { "" };
    QPropertyAnimation *mAnimation { nullptr };
    QPropertyAnimation *mHoverAnimation { nullptr };
};

#endif // MACSWITCHBUTTON_H
