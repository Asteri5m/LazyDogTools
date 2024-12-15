#ifndef CUSTOM_H
#define CUSTOM_H
#include <QMessageBox>
#include "CustomWidget.h"

enum MessageType {
    Info,
    Warning,
    Critical
};

// 通用的显示消息框方法
int showMessage(QWidget *parent, const QString &title, const QString &message,
                 MessageType messageType = Info,
                 const QString &button1Text = "确定", const QString &button2Text = "")
{
    // 创建消息框
    QMessageBox msgBox(parent);

    // 设置消息框的图标
    switch (messageType) {
    case Info:
        msgBox.setIcon(QMessageBox::Information);
        break;
    case Warning:
        msgBox.setIcon(QMessageBox::Warning);
        break;
    case Critical:
        msgBox.setIcon(QMessageBox::Critical);
        break;
    }

    msgBox.setWindowTitle(title);      // 设置窗口标题
    msgBox.setText(message);           // 设置提示消息内容

    // 添加按钮
    MacStyleButton *button1 = new MacStyleButton(button1Text, &msgBox);
    button1->setNormalColorBlue(true);
    msgBox.addButton(button1, QMessageBox::AcceptRole);  // 第一个按钮
    if (!button2Text.isEmpty()) {
        MacStyleButton *button2 = new MacStyleButton(button2Text, &msgBox);
        msgBox.addButton(button2, QMessageBox::RejectRole);  // 第二个按钮
    }

    // 显示消息框
    return msgBox.exec();
}

#endif // CUSTOM_H
