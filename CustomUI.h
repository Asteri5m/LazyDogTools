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

#endif // CUSTOMUI_H
