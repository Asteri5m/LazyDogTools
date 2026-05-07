/**
 * @file CustomKeySequenceEdit.cpp
 * @brief 自定义快捷键输入控件
 */

#include "CustomKeySequenceEdit.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "utils/Custom.h"

CustomKeySequenceEdit::CustomKeySequenceEdit(const QString &text, QWidget *parent)
    : QKeySequenceEdit(parent)
    , mText(text)
{
    // 设置内部 QLineEdit 的样式
    QLineEdit *lineEdit = findChild<QLineEdit *>();
    lineEdit->setFixedHeight(22);
    lineEdit->setStyleSheet("background: transparent; border: none;");
    lineEdit->setTextMargins(MARGIN_TINY, 0, MARGIN_TINY, 0);

    // 设置控件属性
    setAttribute(Qt::WA_Hover);
    addDropShadowEffect(this);

    // 监听主题变化
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &CustomKeySequenceEdit::onThemeChanged);
}

void CustomKeySequenceEdit::onThemeChanged()
{
    update();
}

void CustomKeySequenceEdit::setAlert(bool isAlert, const QString &alertText)
{
    mIsAlert = isAlert;
    mAlertText = alertText;

    QLineEdit *lineEdit = findChild<QLineEdit *>();
    if (!lineEdit) return;

    int leftMargin = MARGIN_MEDIUM;
    int rightMargin = mIsAlert ? 20 : MARGIN_MEDIUM;

    lineEdit->setTextMargins(leftMargin, 0, rightMargin, 0);
    update();
}

void CustomKeySequenceEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
        this->clear();
    }
    else
    {
        QKeySequenceEdit::keyPressEvent(event);
        QString strKeySequence = keySequence().toString().split(",").first();
        QKeySequence seq(QKeySequence::fromString(strKeySequence));

        if (seq.count() == 1 && seq[0].keyboardModifiers() == Qt::NoModifier)
        {
            QKeyCombination keyCombination(Qt::CTRL | Qt::ALT, seq[0].key());
            seq = QKeySequence(keyCombination);
            setAlert(false);
        }

        setKeySequence(seq);
    }

    emit editingFinished();
}

void CustomKeySequenceEdit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    drawBackgroundAndBorder(painter);

    if (mIsAlert)
    {
        drawAlertIcon(painter);
    }

    QLineEdit *lineEdit = findChild<QLineEdit *>();
    lineEdit->setPlaceholderText("");
}

void CustomKeySequenceEdit::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    mIsHovered = true;
    update();
}

void CustomKeySequenceEdit::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    mIsHovered = false;
    update();
}

void CustomKeySequenceEdit::focusInEvent(QFocusEvent *event)
{
    QKeySequenceEdit::focusInEvent(event);
    mHasFocus = true;
    update();
}

void CustomKeySequenceEdit::focusOutEvent(QFocusEvent *event)
{
    QKeySequenceEdit::focusOutEvent(event);
    mHasFocus = false;
    update();
}

void CustomKeySequenceEdit::drawBackgroundAndBorder(QPainter &painter)
{
    painter.setBrush(Qt::white);

    QPen pen;
    pen.setWidth(1);

    if (mHasFocus) {
        pen.setColor(ThemeManager::instance().primaryColor());
        pen.setWidth(2);
    } else if (mIsHovered) {
        pen.setColor(ThemeManager::instance().primaryColor());
    } else if (mIsAlert) {
        pen.setColor(Qt::red);
    } else {
        pen.setColor(QColor(COLOR_BORDER_DARK));
    }

    painter.setPen(pen);
    int radius = RADIUS_SMALL;
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), radius, radius);
}

void CustomKeySequenceEdit::drawAlertIcon(QPainter &painter)
{
    painter.save();

    int iconSize = 14;
    int padding = 4;
    QRect iconRect(width() - iconSize - padding, padding, iconSize, iconSize);

    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(iconRect);

    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(iconSize * 0.7);
    painter.setFont(font);
    painter.drawText(iconRect, Qt::AlignCenter, "!");

    painter.restore();

    if (mIsHovered && !mAlertText.isEmpty())
    {
        QToolTip::showText(mapToGlobal(QPoint(width() - iconSize, iconSize)), mAlertText, this);
    }
}
