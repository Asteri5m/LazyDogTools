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
        setStyleSheet("background-color: #E0E0E0; border-radius: 6px; color: black;");
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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
            painter.setBrush(mPressedColor);
        }
        else if (isChecked())
        {
            painter.setBrush(mCheckedColor);
        }
        else
        {
            painter.setBrush(mNormalColor);
        }
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 6, 6);

        // 绘制文本
        painter.setPen(QColor("black"));
        painter.drawText(rect(), Qt::AlignCenter, text());
    }

    void enterEvent(QEnterEvent *event) override
    {
        setStyleSheet("background-color: #D0D0D0; border-radius: 6px; color: black;");
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        setStyleSheet("background-color: #E0E0E0; border-radius: 6px; color: black;");
        QPushButton::leaveEvent(event);
    }

private:
    // 颜色定义
    QColor mPressedColor{192, 192, 192};  // #C0C0C0
    QColor mCheckedColor{160, 160, 160};  // #A0A0A0
    QColor mNormalColor {224, 224, 224};  // #E0E0E0
};
#endif // CUSTOMWIDGET_H
