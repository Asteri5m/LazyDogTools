#ifndef CUSTOMKEYSEQUENCEEDIT_H
#define CUSTOMKEYSEQUENCEEDIT_H

/**
 * @file CustomKeySequenceEdit.h
 * @brief 自定义快捷键输入控件
 */

#include <QKeySequenceEdit>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLineEdit>
#include <QToolTip>
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QEnterEvent>
#include <QFocusEvent>

class ThemeManager;

class CustomKeySequenceEdit : public QKeySequenceEdit
{
    Q_OBJECT

public:
    explicit CustomKeySequenceEdit(const QString &text = QString(), QWidget *parent = nullptr);

public slots:
    void onThemeChanged();

    void setAlert(bool isAlert, const QString &alertText = QString());
    void setText(const QString &text) { mText = text; }
    QString text() { return mText; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void drawBackgroundAndBorder(QPainter &painter);
    void drawAlertIcon(QPainter &painter);

private:
    bool mIsAlert   { false };
    bool mIsHovered { false };
    bool mHasFocus  { false };
    QString mAlertText  { "" };
    QString mText       { "" };
};

#endif // CUSTOMKEYSEQUENCEEDIT_H
