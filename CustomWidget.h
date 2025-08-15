#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

/**
 * @file CustomWidget.h
 * @author Asteri5m
 * @date 2024-02-07 17:40:10
 * @brief 一些自定义样式的Widget
 */

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QWheelEvent>
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStackedWidget>
#include <QPushButton>
#include <QStyleOption>
#include <QPropertyAnimation>
#include <QSvgRenderer>
#include <QTimer>
#include <stdexcept>

// ToolWidgetModel的菜单栏按钮
class LeftMenuButton : public QPushButton
{
    Q_OBJECT
public:
    explicit LeftMenuButton(const QIcon &icon, const QString &name, QWidget *parent = nullptr)
        : QPushButton(parent) {
        setCheckable(true);
        // 使用栈分配
        QSize buttonSize(56, 56);
        QSize iconSize(24, 24);

        if (name.isNull() && icon.isNull())
        {
            throw std::logic_error("名字和图标至少需要一个");
        }

        // 根据内容适配大小
        if (icon.isNull())
        {
            buttonSize.setHeight(28);
        }
        else if (name.isNull())
        {
            iconSize.setHeight(32);
            iconSize.setWidth(32);
        }

        // 设置按钮大小
        this->setFixedSize(buttonSize);

        // 创建一个垂直布局，图标在上，文本在下
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);

        if (!icon.isNull())
        {
            QLabel *iconLabel = new QLabel(this);
            iconLabel->setPixmap(icon.pixmap(iconSize));
            iconLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(iconLabel);
        }

        if (!name.isNull())
        {
            QLabel *textLabel = new QLabel(name, this);
            textLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(textLabel);
        }
    }
};

// 工具窗口模板，可以根据该模板快生成一个具有左侧菜单栏的“TabWidget”
class ToolWidgetModel : public QWidget
{
    Q_OBJECT
public:
    ToolWidgetModel(QWidget *parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_DeleteOnClose);
    }

    void setDefaultStyle(bool menuRight = false)
    {
        mOverLayout = new QHBoxLayout(this);
        mMainLayout = new QGridLayout();
        mOverLayout->setSpacing(0); // 设置布局中控件之间的间距为0
        mOverLayout->setContentsMargins(0, 0, 0, 0);
        mMainLayout->setContentsMargins(0, 0, 0, 0);

        mMenuWidget = new QWidget();
        mMenuWidget->setFixedWidth(64);

        // 设置菜单的基本样式
        mMenuLayout = new QVBoxLayout(mMenuWidget);
        mMenuLayout->addStretch();
        mMenuLayout->setContentsMargins(0, 0, 0, 0);
        mMenuLayout->setAlignment(Qt::AlignCenter);
        mMenuLayout->setSpacing(15);

        mStackedWidget = new QStackedWidget();
        mMainLayout->addWidget(mStackedWidget);

        QString menuStyle;
        if (menuRight)
        {
            // 设置边框线为1像素宽，灰色
            menuStyle = "QWidget { "
                        "   border-left: 1px solid gray;"
                        "   background-color: white;"
                        "}";
            mOverLayout->addLayout(mMainLayout);
            mOverLayout->addWidget(mMenuWidget);
        }
        else
        {
            menuStyle = "QWidget { "
                        "   border-right: 1px solid gray;"
                        "   background-color: white;"
                        "}";
            mOverLayout->addWidget(mMenuWidget);
            mOverLayout->addLayout(mMainLayout);
        }

        menuStyle += "QPushButton, QPushButton * {"
                     "    background-color: none;"
                     "    border: none;"
                     "}"
                     "QPushButton:hover {"
                     "    background-color: #f0f0f0; "
                     "    border-radius: 6px;"
                     "    border: none;"
                     "}"
                     "QPushButton:checked {"
                     "    background-color: #d0d0d0;"
                     "    border-radius: 6px;"
                     "    border: none;"
                     "}";
        mMenuWidget->setStyleSheet(menuStyle);
    }

    void addTab(QWidget* page, const QIcon &icon=QIcon(), const QString &name=nullptr)
    {
        // 添加按钮
        LeftMenuButton *button = new LeftMenuButton(icon, name);
        button->setStyleSheet("border: none;"); // 清除父类的样式
        mMenuLayout->insertWidget(mMenuLayout->count() - 1, button);
        mButtons.append(button);

        // 添加页面
        mStackedWidget->addWidget(page);

        // 创建关联关系
        connect(button, &LeftMenuButton::clicked, this, [=]() {
            resetButtonStates();
            mStackedWidget->setCurrentWidget(page);
            button->setChecked(true);
        });
    }

    void finalizeSetup()
    {
        if (!mButtons.isEmpty())
        {
            mButtons.first()->click();  // 显示第一个页面
        }
    }

    void showFirstPage()
    {
        if (!mButtons.isEmpty())
            mButtons.first()->click();
        show();
    }

signals:
    void closed();
    void windowEvent(const QString &type, const QString &context);

protected:
    QHBoxLayout *mOverLayout;   // 全局布局，可以在此基础上实现高级自定义
    QGridLayout *mMainLayout;   // 主要布局区域，可以在此基础上实现高级自定义

    //重写关闭信号
    void closeEvent(QCloseEvent *event) override
    {
        emit closed();
        event->accept();
    }

private:
    QWidget        *mMenuWidget;
    QVBoxLayout    *mMenuLayout;
    QStackedWidget *mStackedWidget;
    QVector<LeftMenuButton*> mButtons;

    void resetButtonStates()
    {
        for (auto button = mButtons.constBegin(); button != mButtons.constEnd(); button++)
        {
            (*button)->setChecked(false);
        }
    }
};


// Mac样式的按钮
class MacStyleButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MacStyleButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
        // 设置按钮的样式表
        setStyleSheet(generateStyleSheet(false));  // 默认使用白色样式
        setFixedHeight(24);
        setMinimumWidth(90);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    // 切换蓝色或白色样式
    void setNormalColorBlue(bool isBlue)
    {
        // 重新设置样式表
        setStyleSheet(generateStyleSheet(isBlue));
    }

private:
    // 生成QSS样式表
    QString generateStyleSheet(bool isBlue) const
    {
        // 定义蓝色渐变的线性渐变背景
        QString backgroundColor = isBlue ? "qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #2d8cff, stop:1 #0c75ff)" : "#FFFFFF";
        QString borderColor = isBlue ? "#007AFF" : "#C8C8C8";
        QString fontColor = isBlue ? "white" : "black";
        // QString hoverColor = isBlue ? "#0066CC" : "#0f6aeb";
        QString pressedColor = isBlue ? "#0f6aeb" : "#0f6aeb";

        return QString(
                   "QPushButton {"
                   "   background-color: %1;"
                   "   border: 1px solid %2;"
                   "   border-radius: 6px;"
                   "   color: %3;"
                   "}"
                   "QPushButton:pressed {"
                   "   background-color: %4;"
                   "   color: white;"  // 按下时文本变为白色
                   "}"
                   ).arg(backgroundColor, borderColor, fontColor, pressedColor);
    }
};

// Mac样式开关
class MacSwitchButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(qreal thumbScale READ thumbScale WRITE setThumbScale NOTIFY thumbScaleChanged)

public:
    explicit MacSwitchButton(const QString &text = nullptr, QWidget *parent = nullptr)
        : QWidget(parent), mText(text)
    {
        setFixedSize(40, 20);
        // 切换动画
        mAnimation = new QPropertyAnimation(this, "offset", this);
        mAnimation->setDuration(200);
        mAnimation->setEasingCurve(QEasingCurve::InOutCubic);
        // 鼠标悬浮动画
        mHoverAnimation = new QPropertyAnimation(this, "thumbScale", this);
        mHoverAnimation->setDuration(150);
        mHoverAnimation->setEasingCurve(QEasingCurve::InOutCubic);

        connect(mAnimation, &QPropertyAnimation::finished, this, [=](){
            mIsAnimating = false;
            emit checkedChanged(mChecked);
        });
    }

    qreal offset() const { return mOffset; }
    void setOffset(qreal offset)
    {
        mOffset = offset;
        emit offsetChanged();
        update();
    }

    bool isChecked() const { return mChecked; }
    void setChecked(bool checked)
    {
        if (mChecked == checked) return;
        mChecked = checked;
        mOffset = checked ? 1.0 : 0.0;
        update();
    }

    QString text() { return mText; }

    qreal thumbScale() const { return mThumbScale; }
    void setThumbScale(qreal scale) {
        mThumbScale = scale;
        emit thumbScaleChanged();
        update();
    }

signals:
    void checkedChanged(bool checked);
    void offsetChanged();
    void thumbScaleChanged();

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        QColor bgColor = QColor(mChecked ? "#007AFF" : "#E5E5EA");
        QColor thumbColor = QColor(mChecked ? "#FFFFFF" : "#5E5E5E");
        QColor borderColor = QColor(mChecked ? "#007AFF" : "#5E5E5E"); // 关闭状态边框色为 #5E5E5E

        // 绘制背景
        painter.setPen(QPen(borderColor, 1.5));
        painter.setBrush(bgColor);
        painter.drawRoundedRect(mButtonRect, mRadius, mRadius);

        // 绘制滑块，调整滑块大小，使其略小于背景
        painter.setPen(Qt::NoPen);
        qreal thumbWidth = height() * mThumbScale;
        qreal thumbHeight = height() * mThumbScale;
        qreal thumbX = mOffset * (width() - height()) + (height() - thumbWidth) / 2;
        qreal thumbY = (height() - thumbHeight) / 2;
        QRectF thumbRect(thumbX, thumbY, thumbWidth, thumbHeight);
        painter.setBrush(thumbColor);
        painter.drawEllipse(thumbRect);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && !mIsAnimating) {
            toggleChecked();
        }
    }

    void resizeEvent(QResizeEvent *event) override
    {
        int height = this->height();
        int width = this->width();

        // 确保圆角的半径和高度相匹配
        mRadius = (height) / 2;

        // 更新开关按钮的位置与大小
        int padding = 1;
        mButtonRect = QRect(padding, padding, width - padding * 2, height - padding * 2);

        QWidget::resizeEvent(event);
    }

    void enterEvent(QEnterEvent *event) override {
        mHoverAnimation->stop();
        mHoverAnimation->setStartValue(mThumbScale);
        mHoverAnimation->setEndValue(0.6);
        mHoverAnimation->start();
        QWidget::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        mHoverAnimation->stop();
        mHoverAnimation->setStartValue(mThumbScale);
        mHoverAnimation->setEndValue(0.75);
        mHoverAnimation->start();
        QWidget::leaveEvent(event);
    }

private:
    void toggleChecked()
    {
        if (mIsAnimating) return; // 如果动画正在进行，直接返回
        mIsAnimating = true;

        mChecked = !mChecked;
        mAnimation->setStartValue(mChecked ? 0 : 1);
        mAnimation->setEndValue(mChecked ? 1 : 0);
        mAnimation->start();
    }

private:
    int mRadius { 10 };
    bool mIsAnimating { false };
    bool mChecked { false };
    bool mHovered { false };
    QRect mButtonRect;
    qreal mOffset { 0 };
    qreal mThumbScale { 0.75 };
    QString mText;
    QPropertyAnimation *mAnimation;
    QPropertyAnimation *mHoverAnimation;

};

#include <QGroupBox>
// 自定义GroupBox
class NoBorderGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit NoBorderGroupBox(const QString &title, QWidget *parent = nullptr)
        : QGroupBox(title, parent)
    {

    }

protected:
    // 确保没有绘制边框
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
        QStyleOptionGroupBox opt;
        initStyleOption(&opt);

        // 绘制没有边框的标题
        QFont titleFont;
        titleFont.setBold(true);
        titleFont.setPointSize(10);
        painter.setFont(titleFont);
        painter.drawText(QRect(0, 0, width(), 30), Qt::AlignLeft | Qt::AlignVCenter, this->title());

        // 绘制内容区域
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 30, width(), height() - 30);
    }
};

#include <QScrollArea>
class SmoothScrollArea : public QScrollArea
{
    Q_OBJECT
    Q_PROPERTY(int scrollOffset READ scrollOffset WRITE setScrollOffset)

public:
    SmoothScrollArea(QWidget *parent = nullptr)
        : QScrollArea(parent), mScrollOffset(0), mVelocity(0)
    {
        setFrameStyle(QFrame::NoFrame);
        setAttribute(Qt::WA_TranslucentBackground);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // 这样设置后，进度条背景会很怪，暂时用默认的吧
        // setStyleSheet(R"(
        //     QScrollArea {
        //         background: transparent;
        //     }
        //     QScrollBar:vertical, QScrollBar:horizontal {
        //         background: transparent;
        //         border: none;
        //         width: 8px;
        //     }
        //     QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
        //         background: rgba(0, 0, 0, 25%);
        //         border-radius: 4px;
        //     }
        //     QScrollBar::handle:vertical:hover, QScrollBar::handle:horizontal:hover {
        //         background: rgba(0, 0, 0, 50%);
        //     }
        //     QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
        //     QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
        //         background: transparent;
        //     }
        // )");

        mScrollAnimation = new QPropertyAnimation(this, "scrollOffset");
        mScrollAnimation->setEasingCurve(QEasingCurve::OutQuad);
        mScrollAnimation->setDuration(800);

        mInertiaTimer = new QTimer(this);
        connect(mInertiaTimer, &QTimer::timeout, this, &SmoothScrollArea::onInertiaScroll);
    }

    int scrollOffset() const
    {
        return mScrollOffset;
    }

    void setScrollOffset(int offset)
    {
        int delta = offset - mScrollOffset;
        verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
        mScrollOffset = offset;
    }

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        mVelocity += event->angleDelta().y();
        mScrollAnimation->stop();

        if (!mInertiaTimer->isActive())
        {
            mInertiaTimer->start(16); // 大约 60 FPS
        }
        event->accept();
    }

    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(viewport());

        QRegion region = event->region();
        if (painter.isActive())
        {
            // 只绘制更新区域
            painter.setClipRegion(region);
            QColor bgColor = palette().color(QPalette::Window);
            painter.fillRect(region.boundingRect(), bgColor);
            // 绘制滚动区域内容
        }
    }

private slots:
    void onInertiaScroll()
    {
        mVelocity *= 0.95; // 惯性因子

        if (qAbs(mVelocity) < 1)
        {
            mVelocity = 0;
            mInertiaTimer->stop();
            return;
        }

        int newOffset = mScrollOffset - mVelocity / 10;
        mScrollAnimation->stop();
        setScrollOffset(newOffset);
    }

private:
    int mScrollOffset;
    int mVelocity;
    QPropertyAnimation *mScrollAnimation;
    QTimer *mInertiaTimer;
};

#include <QCheckBox>
// Mac样式的复选框
class MacStyleCheckBox : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(qreal checkBoxAnimationValue READ checkBoxAnimationValue WRITE setCheckBoxAnimationValue)

public:
    MacStyleCheckBox(const QString &text, QWidget *parent = nullptr)
        : QCheckBox(text, parent), m_checkBoxAnimationValue(0.0)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        animation = new QPropertyAnimation(this, "checkBoxAnimationValue", this);
        animation->setDuration(200); // 动画持续时间
        connect(this, &QCheckBox::toggled, this, &MacStyleCheckBox::onToggled);
    }

    qreal checkBoxAnimationValue() const { return m_checkBoxAnimationValue; }
    void setCheckBoxAnimationValue(qreal value)
    {
        m_checkBoxAnimationValue = value;
        update(); // 更新UI
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        // 绘制复选框
        QRect checkboxRect(0, 0, 14, 14);  // 复选框的大小
        checkboxRect.moveTop((height() - checkboxRect.height()) / 2);  // 垂直居中
        checkboxRect.moveLeft(rect().left() + 2);


        QColor uncheckedColor = QColor(255, 255, 255);
        QColor checkedColor = QColor(0, 122, 255);
        QColor borderColor = QColor(94, 94, 94);

        // 根据选中状态设置边框颜色
        painter.setPen(isChecked() ? checkedColor : borderColor);
        painter.setBrush(uncheckedColor);

        // 动画效果
        if (isChecked()) {
            qreal alpha = m_checkBoxAnimationValue;
            QColor animatedColor = checkedColor;
            animatedColor.setAlphaF(alpha);
            painter.setBrush(animatedColor);
        }

        painter.drawRoundedRect(checkboxRect, 3, 3);

        // 绘制勾选标记
        if (isChecked()) {
            QSvgRenderer renderer(QString(":/ico/check_white.svg"));
            QRect checkMarkRect = checkboxRect.adjusted(0, 0, 0, 0);
            renderer.render(&painter, checkMarkRect);
        }

        // 绘制文本
        QRect textRect = rect().adjusted(checkboxRect.height() + 7, 0, -3, 0);  // 文本区域在复选框右侧
        painter.setPen(QColor(0, 0, 0));  // 设置文本颜色
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft , text());  // 绘制文本
    }

private slots:
    void onToggled(bool checked)
    {
        animation->stop();
        animation->setStartValue(checked ? 0.0 : 1.0);
        animation->setEndValue(checked ? 1.0 : 0.0);
        animation->start();
    }

private:
    QPropertyAnimation *animation;
    qreal m_checkBoxAnimationValue;
};

class JumpButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JumpButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
        // 设置按钮大小
        setFixedSize(48, 20);  // 调整尺寸适合图中按钮大小
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        // 绘制背景
        QRect rect = this->rect();
        QColor backgroundColor = isDown()? mCheckedColor: mNormalColor;
        painter.setBrush(backgroundColor);
        painter.setPen(Qt::NoPen); // 无边框
        painter.drawRoundedRect(rect, 5, 5); // 绘制圆角矩形，圆角半径为6px

        // 绘制图标
        QPixmap iconPixmap(":/ico/jump_white.svg");
        QSize iconSize(20, 20);
        QPoint iconPos((width() - iconSize.width()) / 2, (height() - iconSize.height()) / 2);  // 图标居中

        painter.drawPixmap(iconPos, iconPixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

private:
    // 颜色定义
    QColor mCheckedColor{15, 106, 235};  // #0f6aeb
    QColor mNormalColor {0,  122, 255};
};

#include <QKeySequenceEdit>
#include <QKeyEvent>
#include <QToolTip>
#include <QLineEdit>
class CustomKeySequenceEdit : public QKeySequenceEdit
{
    Q_OBJECT

public:
    explicit CustomKeySequenceEdit(const QString &text=QString(), QWidget *parent = nullptr)
        : QKeySequenceEdit(parent)
        , mText(text)
        , mIsAlert(false)
        , mIsHovered(false)
        , mHasFocus(false)
    {
        // 设置内部 QLineEdit 的样式
        QLineEdit *lineEdit = findChild<QLineEdit *>();
        lineEdit->setFixedHeight(22);
        lineEdit->setStyleSheet("background: transparent; border: none;");
        lineEdit->setTextMargins(5, 0, 5, 0); // 左右边距

        // 设置控件属性
        setAttribute(Qt::WA_Hover); // 启用悬停事件检测
    }

    void setAlert(bool isAlert, const QString &alertText = QString())
    {
        mIsAlert = isAlert;
        mAlertText = alertText;

        // 根据告警状态调整边距
        QLineEdit *lineEdit = findChild<QLineEdit *>();
        if (!lineEdit) return;

        int leftMargin = 8;  // 左边距固定
        int rightMargin = mIsAlert ? 20 : 8; // 有告警图标时右边距更大

        lineEdit->setTextMargins(leftMargin, 0, rightMargin, 0);

        update(); // 触发重绘
    }

    void setText(const QString &text) { mText = text; }
    QString text() { return mText; }

protected:
    void keyPressEvent(QKeyEvent *event) override
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

            // 如果输入值是单个按键且没有修饰符，则默认加上 Ctrl+Alt
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

    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        // 绘制背景和边框
        drawBackgroundAndBorder(painter);

        // 如果是告警状态，绘制感叹号图标
        if (mIsAlert)
        {
            drawAlertIcon(painter);
        }

        // 清除提示占位符
        QLineEdit *lineEdit = findChild<QLineEdit *>();
        lineEdit->setPlaceholderText("");
    }

    void enterEvent(QEnterEvent *event) override
    {
        Q_UNUSED(event);
        mIsHovered = true;
        update(); // 触发重绘以显示悬停效果
    }

    void leaveEvent(QEvent *event) override
    {
        Q_UNUSED(event);
        mIsHovered = false;
        update(); // 触发重绘以移除悬停效果
    }

    void focusInEvent(QFocusEvent *event) override
    {
        QKeySequenceEdit::focusInEvent(event);
        mHasFocus = true;
        update(); // 触发重绘以显示焦点效果
    }

    void focusOutEvent(QFocusEvent *event) override
    {
        QKeySequenceEdit::focusOutEvent(event);
        mHasFocus = false;
        update(); // 触发重绘以移除焦点效果
    }

private:
    void drawBackgroundAndBorder(QPainter &painter)
    {
        // 背景色
        painter.setBrush(Qt::white);

        // 边框设置
        QPen pen;
        pen.setWidth(1);

        if (mHasFocus) {
            // 焦点状态边框
            pen.setColor(QColor(0, 122, 255));
            pen.setWidth(2); // 焦点状态边框加粗
        }  else if (mIsHovered) {
            // 悬停状态边框
            pen.setColor(QColor(0, 122, 255));
        } else if (mIsAlert) {
            // 告警状态边框
            pen.setColor(Qt::red);
        }else {
            // 默认状态边框
            pen.setColor(QColor(200, 200, 200));
        }

        painter.setPen(pen);

        // 圆角半径 - 焦点状态时稍大
        int radius = 2;

        // 绘制圆角矩形背景和边框
        painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), radius, radius);
    }

    void drawAlertIcon(QPainter &painter)
    {
        // 保存当前状态
        painter.save();

        // 计算感叹号位置（右上角）
        int iconSize = 14;
        int padding = 4;
        QRect iconRect(width() - iconSize - padding, padding, iconSize, iconSize);

        // 绘制红色圆形背景
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(iconRect);

        // 绘制白色感叹号
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setBold(true);
        font.setPixelSize(iconSize * 0.7);
        painter.setFont(font);
        painter.drawText(iconRect, Qt::AlignCenter, "!");

        // 恢复状态
        painter.restore();

        // 设置感叹号的鼠标提示
        if (mIsHovered && !mAlertText.isEmpty())
        {
            QToolTip::showText(mapToGlobal(QPoint(width() - iconSize, iconSize)), mAlertText, this);
        }
    }

private:
    bool mIsAlert;
    bool mIsHovered;
    bool mHasFocus;
    QString mAlertText;
    QString mText;
};

class HintLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HintLabel(const QString &text, const QString &hint, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel(text.at(0), parent, f)
        , mHint(hint)
    {
        setFixedSize(16, 16);
        setAlignment(Qt::AlignCenter);
        setToolTip(mHint);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        // 启用高质量渲染选项
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);


        // 绘制圆形背景
        painter.setBrush(QColor(0, 122, 255));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(1, 1, 14, 14);

        // 绘制文字
        painter.setPen(Qt::white);

        // 根据设备像素比调整字体大小
        QFont font = painter.font();
        const qreal scale = 1.0 / painter.device()->devicePixelRatioF();
        font.setPixelSize(8 * 2 * scale);
        painter.setFont(font);

        painter.drawText(rect(), Qt::AlignCenter, this->text());
    }

    // 添加按下鼠标也能显示提示
    void mousePressEvent(QMouseEvent *event) override
    {
        QToolTip::showText(event->globalPosition().toPoint(), mHint, this);
    }

private:
    QString mHint;
};

/*******************************
轻量级的自实现 Combo（不继承 QComboBox）
目的：完全控制 popup 的外观（圆角、无系统阴影、边框）绕过原生控件修改困难的问题。

Usage notes:
    MacStyleComboBox *c = new MacStyleComboBox(parent);
    c->addItems({"Apple", "Banana", "Cherry"});
    connect(c, &MacStyleComboBox::activated, [](int idx){ qDebug() << "activated" << idx; });
    已支持：键盘上下选择、回车确认、鼠标点击选择、popup 无系统边框与阴影、圆角+边框、可自定义 item 委托（已内置）。

ToDo：
 - 支持图标或右侧复选标识（在 delegate 中添加图标列）；
 - 支持 model（QAbstractItemModel）直接绑定（把 API 扩展为 setModel()）；
 - 支持自定义样式（颜色/圆角/行高）变成属性；
********************************/

#include <QStringListModel>
#include <QProxyStyle>
#include <QListView>
#include <QStyledItemDelegate>

// Mac样式ComboBox---下拉列表的滚动条样式
class FloatingScrollBarStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const override
    {
        if (control == CC_ScrollBar) {
            const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option);
            if (!slider) {
                QProxyStyle::drawComplexControl(control, option, painter, widget);
                return;
            }

            QScrollBar *scrollbar = qobject_cast<QScrollBar*>(const_cast<QWidget*>(widget));
            if (!scrollbar) return;

            int range = scrollbar->maximum() - scrollbar->minimum();
            if (range < 0) return;

            const int padding = 2; // 上下padding

            int trackLength = slider->rect.height() - 2 * padding; // 减去padding，滚动条轨道实际绘制长度
            int handleLength = qMax(trackLength * slider->pageStep / (range + slider->pageStep), 20);

            // 计算滑块位置时，要加上padding
            int handlePos = (trackLength - handleLength) * (scrollbar->value() - scrollbar->minimum()) / (range > 0 ? range : 1);

            painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter->setPen(Qt::NoPen);
            painter->setBrush(Qt::white);
            painter->drawRect(slider->rect);

            // 滑块的矩形，上下都要加padding，注意减去padding产生的长度变化
            QRect handleRect(slider->rect.right() - 8, slider->rect.top() + padding + handlePos, 6, handleLength);
            QColor color(102, 102, 102);
            color.setAlpha(slider->state & State_MouseOver ? 255 : 204); // hover 不透明

            painter->save();
            painter->setBrush(color);
            painter->drawRoundedRect(handleRect, 3, 3);
            painter->restore();

            return;
        }

        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
};

// Mac样式ComboBox---Delegate
class ComboBoxDelegate : public QStyledItemDelegate
{
public:
    explicit ComboBoxDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
        , mCheckWhite(QLatin1String(":/ico/check_white.svg"))
        , mCheckBlack(QLatin1String(":/ico/check_black.svg"))
    {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        QRect baseRect = option.rect.adjusted(5, 1, -5, -1);
        bool isHovered = option.state & QStyle::State_MouseOver;

        // 在自绘的 list 中，选中项通常由 State_Selected 表示
        bool isSelected = option.state & QStyle::State_Selected;

        // 画√区域，固定在左边
        QRect checkRect(baseRect.left() + 3, baseRect.top() + (baseRect.height() - 14)/2, 14, 14);

        // 绘制hover背景，左右留空，圆角矩形
        if (isHovered) {
            painter->setBrush(QColor(0, 122, 255));
            painter->setPen(Qt::NoPen);
            painter->drawRoundedRect(baseRect, 4, 4);
            if (isSelected) mCheckWhite.render(painter, checkRect);
        } else if (isSelected) {
            mCheckBlack.render(painter, checkRect);
        } else {
            painter->fillRect(option.rect, option.backgroundBrush);
        }

        // 文字区域：留出√宽度+间距
        int textLeft = baseRect.left() + 18;
        QRect textRect(textLeft, baseRect.top(), baseRect.width() - (textLeft - baseRect.left()), baseRect.height());
        painter->setPen(isHovered ? Qt::white : Qt::black);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data().toString());
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(22);
        return size;
    }

private:
    mutable QSvgRenderer mCheckWhite;
    mutable QSvgRenderer mCheckBlack;
};

// Mac样式ComboBox---自绘的 popup 容器（无系统阴影，圆角边框）
class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr)
        : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);

        mListView = new QListView(this);
        mListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        mListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mListView->setSelectionMode(QAbstractItemView::SingleSelection);
        mListView->setUniformItemSizes(true);

        QVBoxLayout *l = new QVBoxLayout(this);
        l->setContentsMargins(6, 6, 6, 6); // 留出边框和圆角
        l->setSpacing(0);
        l->addWidget(mListView);
        setLayout(l);

        // 防止 popup 获得焦点后主窗口样式出问题
        setFocusPolicy(Qt::NoFocus);
    }

    QListView* listView() const { return mListView; }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter p(this);
        p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

        QRect r = rect().adjusted(0, 0, -1, -1);
        QPainterPath path;
        path.addRoundedRect(r, 6, 6);

        // 背景
        p.fillPath(path, Qt::white);

        // 边框
        QPen pen(QColor(200, 200, 200));
        pen.setWidthF(1.0);
        p.setPen(pen);
        p.drawPath(path);
    }

private:
    QListView *mListView;
};

// Mac样式ComboBox---自定义实现，非继承 QComboBox
class MacStyleComboBox : public QWidget
{
    Q_OBJECT
public:
    explicit MacStyleComboBox(QWidget *parent = nullptr) : MacStyleComboBox(QString(), parent) { }

    explicit MacStyleComboBox(const QString &text = QString(), QWidget *parent = nullptr)
        : QWidget(parent)
        , mText(text)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(23);

        mPopup = new PopupWidget(this);
        mModel = new QStringListModel(this);
        mPopup->listView()->setModel(mModel);
        mPopup->listView()->setItemDelegate(new ComboBoxDelegate(mPopup->listView()));
        mPopup->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

        // 给 listview 一个小样式替换
        mPopup->listView()->setFrameShape(QFrame::NoFrame);
        mPopup->listView()->setLineWidth(0);
        mPopup->listView()->setMidLineWidth(0);
        mPopup->listView()->verticalScrollBar()->setStyle(new FloatingScrollBarStyle());

        connect(mPopup->listView(), &QListView::clicked, this, &MacStyleComboBox::onListClicked);
    }

    // 设置与获取组件名，便于识别
    void setText(const QString &text){ mText = text; }
    const QString text()             { return mText; }

    // 基础API
    void addItem(const QString &text)
    {
        int r = mModel->rowCount();
        mModel->insertRow(r);
        mModel->setData(mModel->index(r), text);
        if (mCurrentIndex < 0) setCurrentIndex(0);
    }

    void addItems(const QStringList &list)
    {
        for (const QString &s : list) addItem(s);
    }

    void clear()
    {
        mModel->removeRows(0, mModel->rowCount());
        mCurrentIndex = -1;
        update();
    }

    int count() const { return mModel->rowCount(); }
    QString itemText(int row) const { return mModel->data(mModel->index(row)).toString(); }

    QString currentText() const { return (mCurrentIndex >= 0 && mCurrentIndex < mModel->rowCount()) ? itemText(mCurrentIndex) : QString(); }
    int currentIndex() const { return mCurrentIndex; }

public slots:
    void setCurrentIndex(int idx)
    {
        if (idx < 0 || idx >= mModel->rowCount()) return;
        if (mCurrentIndex == idx) return;
        mCurrentIndex = idx;
        update();
        emit currentIndexChanged(mCurrentIndex);
        emit currentTextChanged(currentText());
    }

    void setCurrentText(const QString &text)
    {
        int index = -1; // 查找匹配项
        for (int i = 0; i < mModel->rowCount(); ++i) {
            QModelIndex idx = mModel->index(i, 0);
            if (mModel->data(idx, Qt::DisplayRole).toString() == text) {
                index = i;
                break;
            }
        }

        setCurrentIndex(index);
    }

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString &text);
    void activated(int index);

protected:
    // 绘制主按钮（仿 Mac 风格）
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        QRect rect = this->rect().adjusted(1, 1, -1, -1);
        int radius = 4;

        // 外层边框 + 背景
        painter.setBrush(QColor(250, 250, 250));
        painter.setPen(QColor(200, 200, 200));
        painter.drawRoundedRect(rect, radius, radius);

        // 内容区域白底
        painter.setBrush(QColor(255, 255, 255));
        painter.setPen(QColor(200, 200, 200));
        painter.drawRoundedRect(rect, radius, radius);

        // 文本
        painter.setPen(QColor(0, 0, 0));
        QRect textRect = rect.adjusted(8, 0, -22, 0);
        QFontMetrics fm = painter.fontMetrics();
        QString elided = fm.elidedText(currentText(), Qt::ElideRight, textRect.width());
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

        // 箭头的大小和位置设置
        int arrowSize = rect.height() * 0.85;
        int arrowX = rect.width() - arrowSize - 1;
        int arrowY = (rect.height() - arrowSize) / 2 + 1;
        QRect arrowRect(arrowX, arrowY, arrowSize, arrowSize);

        // 背景颜色加深
        QColor arrowBackgroundColor = mPressed ? QColor(15, 106, 235) : QColor(0, 122, 255);  // 按下时变深
        painter.setBrush(arrowBackgroundColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(arrowRect, radius, radius);

        QSvgRenderer renderer(QString(":/ico/arrow_white.svg"));
        painter.setPen(Qt::NoPen);
        renderer.render(&painter, arrowRect);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            mPressed = true;
            update();
        }
        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (mPressed)
        {
            mPressed = false;
            update();
            // 点击后切换 popup
            if (rect().contains(event->pos())) {
                togglePopup();
            }
        }
        QWidget::mouseReleaseEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Down) {
            if (!mPopup->isVisible()) showPopup();
            else navigate(1);
            return;
        }
        if (event->key() == Qt::Key_Up) {
            if (mPopup->isVisible()) navigate(-1);
            return;
        }
        if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && mPopup->isVisible()) {
            QModelIndex idx = mPopup->listView()->currentIndex();
            if (idx.isValid()) {
                setCurrentIndex(idx.row());
                hidePopup();
                emit activated(mCurrentIndex);
            }
            return;
        }

        QWidget::keyPressEvent(event);
    }

    QSize sizeHint() const override
    {
        QFontMetrics fm(font());

        int maxTextWidth = 40;
        for (int i = 0; i < mModel->rowCount(); ++i) {
            QString text = mModel->data(mModel->index(i, 0), Qt::DisplayRole).toString();
            int w = fm.horizontalAdvance(text);
            if (w > maxTextWidth)
                maxTextWidth = w;
        }

        const int padding = 38;  //因为最右边有按钮
        int width = maxTextWidth + padding;

        // 限制最大宽度
        width = qMin(width, 1000);

        int height = fm.height();

        return QSize(width, height);
    }

    QSize minimumSizeHint() const override {
        // 获取原始的 sizeHint
        QSize originalSize = sizeHint();     // 边距

        int width = originalSize.width();
        if (width > 200)  width = 200;      // 限制下避免太长
        int height = originalSize.height();

        return QSize(width, height);
    }

private slots:
    void onListClicked(const QModelIndex &index)
    {
        if (!index.isValid()) return;
        setCurrentIndex(index.row());
        hidePopup();
        emit activated(mCurrentIndex);
    }

private:
    void showPopup()
    {
        if (mModel->rowCount() == 0)
            return;

        auto *listView = mPopup->listView();
        QFontMetrics fm(listView->font());

        // 计算最大文本宽度
        int maxTextWidth = 0;
        for (int row = 0; row < mModel->rowCount(); ++row) {
            QString text = mModel->data(mModel->index(row, 0), Qt::DisplayRole).toString();
            maxTextWidth = qMax(maxTextWidth, fm.horizontalAdvance(text));
        }

        // 预留宽度（勾选框 + padding）
        int padding = 50;

        // 基本宽度
        int popupWidth = maxTextWidth + padding;

        // 计算高度
        int rowHeight = listView->sizeHintForRow(0);
        if (rowHeight <= 0) rowHeight = fm.height() + 8;

        const int maxVisibleRows = 9;   // 奇数个，方便上下对齐、对称
        int visibleRows = qMin(maxVisibleRows, mModel->rowCount());
        int popupHeight = visibleRows * rowHeight + 12; // 包含边距

        // 如果条目多于最大可见行，预估滚动条宽度，额外加宽
        if (mModel->rowCount() > maxVisibleRows)
            popupWidth += 15;

        // 设置尺寸
        listView->setMinimumWidth(popupWidth - 12);
        mPopup->resize(popupWidth, popupHeight);

        // 计算弹窗位置
        QPoint globalPos = this->mapToGlobal(QPoint(0, this->height()));

        // 取当前选中项，滚动并居中
        if (mCurrentIndex >= 0) {
            QModelIndex curIndex = mModel->index(mCurrentIndex);
            listView->setCurrentIndex(curIndex);
            listView->scrollTo(curIndex, QAbstractItemView::PositionAtCenter);

            QRect itemRect = listView->visualRect(curIndex);
            globalPos -= QPoint(itemRect.left() + 20, itemRect.top() + rowHeight + 8);
        }

        // 移动弹窗到目标位置，显示
        mPopup->move(globalPos);
        mPopup->show();
        mPopup->raise();
        listView->setFocus();
    }

    void hidePopup()
    {
        mPopup->hide();
    }

    void togglePopup()
    {
        if (mPopup->isVisible()) hidePopup();
        else showPopup();
    }

    int calcPopupWidth() const
    {
        QFontMetrics fm(font());
        int maxw = 0;
        for (int i = 0; i < mModel->rowCount(); ++i) {
            QString t = mModel->data(mModel->index(i)).toString();
            int w = fm.horizontalAdvance(t);
            if (w > maxw) maxw = w;
        }
        return maxw + 45; // 留出勾选/边距空间
    }

    void navigate(int step)
    {
        int n = mModel->rowCount();
        if (n == 0) return;
        int idx = mPopup->listView()->currentIndex().row();
        if (idx < 0) idx = (mCurrentIndex >= 0) ? mCurrentIndex : 0;
        idx = qBound(0, idx + step, n - 1);
        QModelIndex newIdx = mModel->index(idx);
        mPopup->listView()->setCurrentIndex(newIdx);
        mPopup->listView()->scrollTo(newIdx, QAbstractItemView::PositionAtCenter);
    }

private:
    QString mText;
    PopupWidget *mPopup;
    QStringListModel *mModel;
    int mCurrentIndex { -1 };
    bool mPressed { false };
};

#endif // CUSTOMWIDGET_H
