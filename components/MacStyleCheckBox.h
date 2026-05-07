#ifndef MACSTYLECHECKBOX_H
#define MACSTYLECHECKBOX_H

/**
 * @file MacStyleCheckBox.h
 * @brief Mac样式的复选框
 */

#include <QCheckBox>
#include <QPainter>
#include <QRect>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <QSvgRenderer>


class MacStyleCheckBox : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(qreal checkBoxAnimationValue READ checkBoxAnimationValue WRITE setCheckBoxAnimationValue)

public:
    MacStyleCheckBox(const QString &text, QWidget *parent = nullptr);

public slots:
    void onThemeChanged();

    qreal checkBoxAnimationValue() const { return mCheckBoxAnimationValue; }
    void setCheckBoxAnimationValue(qreal value);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onToggled(bool checked);

private:
    QPropertyAnimation *mAnimation { nullptr };
    qreal mCheckBoxAnimationValue  { 0.0 };
};

#endif // MACSTYLECHECKBOX_H
