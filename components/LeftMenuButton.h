#ifndef LEFTMENUBUTTON_H
#define LEFTMENUBUTTON_H

/**
 * @file LeftMenuButton.h
 * @brief ToolWidgetModel的菜单栏按钮
 */

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QSize>
#include <stdexcept>
#include "utils/Constants.h"

class LeftMenuButton : public QPushButton
{
    Q_OBJECT

public:
    explicit LeftMenuButton(const QString &iconPath, const QString &name, QWidget *parent = nullptr);

public slots:
    void toggleIcon();
    void setChecked(bool checked);

private:
    QString mIconPath;
    QString mName;
    QSize mIconSize;
    QLabel *mIconLabel = nullptr;
    QLabel *mTextLabel = nullptr;
    QIcon *mNormalIcon  = nullptr;
    QIcon *mCheckedIcon = nullptr;
    QString mNormalColor  = COLOR_TEXT_PRIMARY;
    QString mCheckedColor = COLOR_TEXT_SELECTED;
};

#endif // LEFTMENUBUTTON_H
