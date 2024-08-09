#ifndef CUSTOMUI_H
#define CUSTOMUI_H

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
#include <stdexcept>

// 主程序窗口的工具列表子元素窗口
class MinToolWidget : public QWidget {
    Q_OBJECT

public:
    MinToolWidget(const int id, const QString &iconPath, const QString &name, const QString &description, QWidget *parent = nullptr)
        : QWidget(parent) {
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

protected:
    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event);
        mHovering = true;
        update(); // 触发重绘
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event);
        mHovering = false;
        update(); // 触发重绘
    }

    void mousePressEvent(QMouseEvent *event) override {
        mIsPressed = true;
        update(); // 触发重绘
        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        mIsPressed = false;
        update(); // 触发重绘
        QWidget::mouseReleaseEvent(event);
    }

    void paintEvent(QPaintEvent *event) override {
        QWidget::paintEvent(event);

        if (mHovering && !mIsPressed) {
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
class LeftMenuButton : public QPushButton {

public:
    explicit LeftMenuButton(const QIcon &icon, const QString &name, QWidget *parent = nullptr)
        : QPushButton(parent) {
        QSize* buttonSize = new QSize(56, 56);
        QSize* iconSize   = new QSize(24, 24);

        if (name.isNull() && icon.isNull()) {
            throw std::logic_error("名字和图标至少需要一个");
        }

        // 根据内容适配大小
        if (icon.isNull()) {
            buttonSize->setHeight(28);
        } else if (name.isNull()) {
            iconSize->setHeight(32);
            iconSize->setWidth(32);
        }

        // 设置按钮大小
        this->setFixedSize(*buttonSize);

        // 去除选中效果
        this->setStyleSheet("QPushButton { border: none; }");

        // 创建一个垂直布局，图标在上，文本在下
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);

        if (!icon.isNull()) {
            QLabel *iconLabel = new QLabel(this);
            iconLabel->setPixmap(icon.pixmap(*iconSize));
            iconLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(iconLabel);
        }

        if (!name.isNull()) {
            QLabel *textLabel = new QLabel(name, this);
            textLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(textLabel);
        }
    }

protected:
    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event);
        setStyleSheet("background-color: #f0f0f0;"
                      "border-radius: 3px;"
                      "border: none");
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event);
        setStyleSheet("background-color: none");
        setStyleSheet("border: none");
    }

};

// 工具窗口模板，可以根据该模板快生成一个具有左侧菜单栏的“TabWidget”
// template<typename Derived>
class ToolWidgetModel : public QWidget {
public:
    ToolWidgetModel(QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        mMenuWidget = new QWidget(this);
        mMenuWidget->setFixedWidth(64);
        mMenuWidget->setStyleSheet("border-right: 1px solid gray;"
                                   "background-color: white;");   // 设置边框线为1像素宽，灰色

        // 设置菜单的基本样式
        mMenuLayout = new QVBoxLayout(mMenuWidget);
        mMenuLayout->addStretch();
        mMenuLayout->setContentsMargins(0, 0, 0, 0);
        mMenuLayout->setAlignment(Qt::AlignCenter);
        mMenuLayout->setSpacing(15);

        mStackedWidget = new QStackedWidget(this);

        mainLayout->addWidget(mMenuWidget);
        mainLayout->addWidget(mStackedWidget);
        setLayout(mainLayout);
    }

    void addTab(QWidget* page, const QIcon &icon=QIcon(), const QString &name=nullptr) {
        // 添加按钮
        LeftMenuButton *button = new LeftMenuButton(icon, name);
        button->setStyleSheet("border: none;"); // 清除父类的样式
        mMenuLayout->insertWidget(mMenuLayout->count() - 1, button);
        mButtons.append(button);

        // 添加页面
        mStackedWidget->addWidget(page);

        // 创建关联关系
        connect(button, &LeftMenuButton::clicked, [=]() {
            resetButtonStates();
            mStackedWidget->setCurrentWidget(page);
            button->setChecked(true);
        });
    }

    void finalizeSetup() {
        if (!mButtons.isEmpty()) {
            mButtons.first()->click();  // 显示第一个页面
        }
    }

private:
    QWidget        *mMenuWidget;
    QVBoxLayout    *mMenuLayout;
    QStackedWidget *mStackedWidget;
    QVector<LeftMenuButton*> mButtons;

    void resetButtonStates() {
        for (auto button : mButtons) {
            button->setChecked(false);
        }
    }
};

#endif // CUSTOMUI_H
