#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

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
#include <stdexcept>

// 主程序窗口的工具列表子元素窗口
class MinToolWidget : public QWidget
{
    Q_OBJECT

public:
    MinToolWidget(const int id, const QString &iconPath, const QString &name, const QString &description, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);

        // 根据id获取位置
        bool iconOnLeft = (id % 2 == 0);

        // 生成icon
        QLabel *iconLabel = new QLabel(this);
        iconLabel->setPixmap(QPixmap(iconPath).scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // 生成名称和描述
        QLabel *nameLabel = new QLabel(name, this);
        nameLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
        nameLabel->setContentsMargins(0, 0, 0, 0);
        QLabel *descriptionLabel = new QLabel(description, this);
        descriptionLabel->setWordWrap(true);
        descriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        // 将名称和描述设置为组
        QVBoxLayout *textLayout = new QVBoxLayout;
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(descriptionLabel);

        // 设置大小策略
        QWidget *textWidget = new QWidget(this);
        textWidget->setLayout(textLayout);
        textWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // 实现位置交替
        if (iconOnLeft) {
            mainLayout->addWidget(iconLabel);
            mainLayout->addWidget(textWidget);
        } else {
            mainLayout->addWidget(textWidget);
            mainLayout->addWidget(iconLabel);
        }

        setLayout(mainLayout);
    }

signals:
    void widgetDoubleClicked(); // 自定义信号: 双击

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override
    {
        emit widgetDoubleClicked(); // 发射自定义信号
        // 调用基类的实现以保持默认行为
        QWidget::mouseDoubleClickEvent(event);
    }

    void enterEvent(QEnterEvent *event) override
    {
        Q_UNUSED(event);
        mHovering = true;
        update(); // 触发重绘
    }

    void leaveEvent(QEvent *event) override
    {
        Q_UNUSED(event);
        mHovering = false;
        update(); // 触发重绘
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        mIsPressed = true;
        update(); // 触发重绘
        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        mIsPressed = false;
        update(); // 触发重绘
        QWidget::mouseReleaseEvent(event);
    }

    void paintEvent(QPaintEvent *event) override
    {
        QWidget::paintEvent(event);

        if (mHovering && !mIsPressed)
        {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿
            painter.setBrush(QColor(0, 0, 0, 50)); // 半透明黑色
            painter.setPen(Qt::NoPen);

            QPainterPath path;
            path.addRoundedRect(rect(), 5, 5); // 设置圆角半径为 5px

            painter.drawPath(path); // 绘制遮罩
        }
    }

private:
    bool mHovering = false;
    bool mIsPressed = false;

};

// ToolWidgetModel的菜单栏按钮
class LeftMenuButton : public QPushButton
{
    Q_OBJECT
public:
    explicit LeftMenuButton(const QIcon &icon, const QString &name, QWidget *parent = nullptr)
        : QPushButton(parent) {
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

    void setChecked(bool checked)
    {
        isChecked = checked;
        if (isChecked)
        {
            setStyleSheet("background-color: #d0d0d0;"
                          "border-radius: 6px;"
                          "border: none;");
        }
        else
        {
            setStyleSheet("background-color: none;"
                          "border: none;");
        }
    }

protected:
    void enterEvent(QEnterEvent *event) override
    {
        Q_UNUSED(event);
        if (!isChecked)
        {
            setStyleSheet("background-color: #f0f0f0;"
                          "border-radius: 6px;"
                          "border: none;");
        }
    }

    void leaveEvent(QEvent *event) override
    {
        Q_UNUSED(event);
        if (!isChecked)
        {
            setStyleSheet("background-color: none;"
                          "border: none;");
        }
    }

private:
    bool isChecked = false;

};

// 工具窗口模板，可以根据该模板快生成一个具有左侧菜单栏的“TabWidget”
class ToolWidgetModel : public QWidget
{
public:
    ToolWidgetModel(QWidget *parent = nullptr) : QWidget(parent) {}

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

        if (menuRight)
        {
            mMenuWidget->setStyleSheet("border-left: 1px solid gray;"
                                       "background-color: white;");   // 设置边框线为1像素宽，灰色
            mOverLayout->addLayout(mMainLayout);
            mOverLayout->addWidget(mMenuWidget);
        }
        else
        {
            mMenuWidget->setStyleSheet("border-right: 1px solid gray;"
                                       "background-color: white;");   // 设置边框线为1像素宽，灰色
            mOverLayout->addWidget(mMenuWidget);
            mOverLayout->addLayout(mMainLayout);
        }
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

protected:
    QHBoxLayout *mOverLayout;   // 全局布局，可以在此基础上实现高级自定义
    QGridLayout *mMainLayout;   // 主要布局区域，可以在此基础上实现高级自定义

private:
    QWidget        *mMenuWidget;
    QVBoxLayout    *mMenuLayout;
    QStackedWidget *mStackedWidget;
    QVector<LeftMenuButton*> mButtons;

    void resetButtonStates()
    {
        for (auto button : mButtons)
        {
            button->setChecked(false);
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
        // 设置默认样式
        setFixedHeight(24);
        setMinimumWidth(90);
        // setStyleSheet("background-color: #E0E0E0; border: 1px solid #007AFF; border-radius: 6px; color: black;");
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    // 按钮的两种基本颜色，白色、蓝色
    void setNormalColorBlue(bool isBlue)
    {
        if (isBlue)
        {
            mNormalColor = QColor(0, 122, 255);
            mFontColor   = QColor("white");
        }
        else
        {
            mNormalColor = QColor(255, 255, 255);
            mFontColor   = QColor("black");
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QStyleOptionButton option;
        option.initFrom(this);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制背景
        if (isDown())
        {
            painter.setBrush(mCheckedColor);
        }
        else
        {
            painter.setBrush(mNormalColor);
        }
        painter.setPen(QColor(200, 200, 200)); // 边框颜色
        painter.drawRoundedRect(rect(), 6, 6);

        // 绘制文本
        painter.setPen(mFontColor);
        painter.drawText(rect(), Qt::AlignCenter, text());
    }

private:
    // 颜色定义
    QColor mCheckedColor{15,  106, 235};  // #0f6aeb
    QColor mNormalColor {255, 255, 255};
    // 字体颜色
    QColor mFontColor{"black"};
};

// Mac样式开关
class MacSwitchButton : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)

public:
    explicit MacSwitchButton(const QString &text = nullptr, QWidget *parent = nullptr)
        : QWidget(parent), mOffset(0), mRadius(10), mIsAnimating(false), mChecked(false)
    {
        mText = text;
        setFixedSize(40, 20);
        mAnimation = new QPropertyAnimation(this, "offset", this);
        mAnimation->setDuration(200);
        mAnimation->setEasingCurve(QEasingCurve::InOutCubic);

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

signals:
    void checkedChanged(bool checked);
    void offsetChanged();

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QColor bgColor = QColor(mChecked ? "#007AFF" : "#E5E5EA");
        QColor thumbColor = QColor(255, 255, 255);

        // 绘制背景
        painter.setBrush(bgColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(mButtonRect, mRadius, mRadius);

        // 绘制滑块，调整滑块大小，使其略小于背景
        qreal thumbWidth = height() * 0.9;  // 滑块宽度为背景高度的90%
        qreal thumbHeight = height() * 0.9; // 滑块高度为背景高度的90%
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
        mRadius = height / 2;

        // 更新开关按钮的位置与大小
        mButtonRect = QRect(0, 0, width, height);

        QWidget::resizeEvent(event);
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
    qreal mOffset;
    int mRadius;
    QRect mButtonRect;
    QPropertyAnimation *mAnimation;
    bool mIsAnimating;
    bool mChecked;
    QString mText;
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
#include <QTimer>
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


#include <QComboBox>
#include <QStyleOptionComboBox>
#include <QSvgRenderer>
#include <QStylePainter>
// Mac样式下拉框
class MacStyleComboBox : public QComboBox
{
    Q_OBJECT

public:
    MacStyleComboBox(QString name = "", QWidget *parent = nullptr)
        : QComboBox(parent)
    {
        mName = name;
        setStyleSheet(
            "MacStyleComboBox {"
            "   border: 1px solid #CCCCCC;"
            "   border-radius: 6px;"
            "   padding: 3px;"
            "   background-color: white;"
            "   selection-background-color: #007AFF;"
            "   selection-color: white;"
            "   color: black;"  // 设置文字颜色为黑色
            "}"
            "QComboBox QAbstractItemView {"
            "   background-color: rgba(255, 255, 255, 0.8);"
            "   border-radius: 6px;"
            "   border: 1px solid #CCCCCC;;"
            "   selection-background-color: #fff;"
            "   selection-color: white;"
            "}"
            "QComboBox::item {"
            "   padding: 5px;"
            "}"
            );
    }

    // 设置与获取组件名，便于识别
    void setName(QString name){ mName = name; }
    QString getName()         { return mName; }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRect rect = this->rect();
        rect.setHeight(24);

        // 背景颜色设定
        painter.setBrush(QColor(255, 255, 255));
        painter.setPen(QColor(200, 200, 200));  // 边框颜色
        painter.drawRoundedRect(rect, 6, 6);  // 绘制按钮背景

        // 绘制文字
        painter.setPen(QColor(0, 0, 0));
        painter.drawText(rect.adjusted(10, 0, -25, 0), Qt::AlignVCenter, currentText());

        // 箭头的大小和位置设置
        int arrowSize = rect.height() * 0.7;
        int arrowX = rect.width() - arrowSize - 5;
        int arrowY = (rect.height() - arrowSize) / 2;
        QRect arrowRect(arrowX, arrowY, arrowSize, arrowSize);

        // 背景颜色加深
        QColor arrowBackgroundColor = mIsPressed ? QColor(15, 106, 235) : QColor(0, 122, 255);  // 按下时变深
        painter.setBrush(arrowBackgroundColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(arrowRect, 6, 6);

        QSvgRenderer renderer(QString(":/ico/arrow_white.svg"));
        painter.setPen(Qt::NoPen);
        renderer.render(&painter, arrowRect);

    }

    void mousePressEvent(QMouseEvent *event) override
    {
        // 自定义操作：改变按钮的颜色等
        if (event->button() == Qt::LeftButton)
        {
            mIsPressed = true;; // 假设这是您用于改变按钮颜色的函数
            update(); // 重新绘制按钮以反映颜色变化
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        mIsPressed = false;
        update();  // 更新UI
        // 调用基类的 mousePressEvent 以确保正常的下拉操作
        QComboBox::mousePressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override
    {
        // 忽略滚轮事件，防止滚轮改变当前选项
        event->ignore();
    }


private:
    bool mIsPressed = false;  // 记录按钮是否按下
    QString mName;
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
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制复选框
        QRect checkboxRect(0, 0, 16, 16);  // 复选框的大小
        checkboxRect.moveTop((height() - checkboxRect.height()) / 2);  // 垂直居中
        checkboxRect.moveLeft(rect().left() + 1);


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

        painter.drawRoundedRect(checkboxRect, 5, 5);

        // 绘制勾选标记
        if (isChecked()) {
            QSvgRenderer renderer(QString(":/ico/check_white.svg"));
            QRect checkMarkRect = checkboxRect.adjusted(0, 0, 0, 0);
            renderer.render(&painter, checkMarkRect);
        }

        // 绘制文本
        QRect textRect = rect().adjusted(checkboxRect.height() + 5, 0, -3, 0);  // 文本区域在复选框右侧
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
        painter.setRenderHint(QPainter::Antialiasing);

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



#endif // CUSTOMWIDGET_H
