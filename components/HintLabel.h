#ifndef HINTLABEL_H
#define HINTLABEL_H

/**
 * @file HintLabel.h
 * @brief 提示标签
 */

#include <QLabel>
#include <QPainter>
#include <QRect>
#include <QFont>
#include <QEnterEvent>
#include <QToolTip>

class ThemeManager;

class HintLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HintLabel(const QString &text, const QString &hint, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void onThemeChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString mHint { "" };
};

#endif // HINTLABEL_H
