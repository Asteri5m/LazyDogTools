/**
 * @file LeftMenuButton.cpp
 * @brief ToolWidgetModel的菜单栏按钮
 */

#include "LeftMenuButton.h"
#include "utils/Custom.h"

LeftMenuButton::LeftMenuButton(const QString &iconPath, const QString &name, QWidget *parent)
    : QPushButton(parent), mIconPath(iconPath), mName(name)
{
    setCheckable(true);

    QSize buttonSize(56, 56);
    mIconSize = QSize(24, 24);

    if (name.isNull() && iconPath.isNull()) {
        throw std::logic_error("名字和图标至少需要一个");
    }

    if (iconPath.isNull()) {
        buttonSize.setHeight(28);
    } else if (name.isNull()) {
        mIconSize.setHeight(32);
        mIconSize.setWidth(32);
    }

    this->setFixedSize(buttonSize);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);

    if (!iconPath.isNull()) {
        mNormalIcon  = new QIcon(iconPath + ".svg");
        mCheckedIcon = new QIcon(iconPath + "_check.svg");
        mIconLabel = new QLabel(this);
        mIconLabel->setPixmap(mNormalIcon->pixmap(mIconSize));
        mIconLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(mIconLabel);
    }

    if (!name.isNull()) {
        mTextLabel = new QLabel(name, this);
        mTextLabel->setAlignment(Qt::AlignCenter);
        mTextLabel->setStyleSheet(QString("color: %1;").arg(mNormalColor));
        layout->addWidget(mTextLabel);
    }

    addDropShadowEffect(this);

    connect(this, &QPushButton::clicked, this, &LeftMenuButton::toggleIcon);
}

void LeftMenuButton::toggleIcon()
{
    if (isChecked()) {
        if(!mIconPath.isNull()) mIconLabel->setPixmap(mCheckedIcon->pixmap(mIconSize));
        if(!mName.isNull()) mTextLabel->setStyleSheet(QString("color: %1;").arg(mCheckedColor));
    } else {
        if(!mIconPath.isNull()) mIconLabel->setPixmap(mNormalIcon->pixmap(mIconSize));
        if(!mName.isNull()) mTextLabel->setStyleSheet(QString("color: %1;").arg(mNormalColor));
    }
}

void LeftMenuButton::setChecked(bool checked)
{
    QPushButton::setChecked(checked);
    toggleIcon();
}
