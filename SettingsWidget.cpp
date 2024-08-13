#include "SettingsWidget.h"

SettingsWidget::SettingsWidget(QWidget *parent)
    : ToolWidgetModel{parent}
{
    setFixedSize(630, 425);
    // 取消其他按钮，只保留关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    // 使用默认模板样式
    setDefaultStyle();

    page1 = new QWidget(this);
    page2 = new QWidget(this);
    page3 = new QWidget(this);

    addTab(page1, QIcon(":/ico/LD.ico"), "基础");
    addTab(page2, QIcon(":/ico/LD.ico"), "应用");
    addTab(page3, QIcon(":/ico/LD.ico"), "热键");

    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    QVBoxLayout *layout3 = new QVBoxLayout(page3);

    layout1->addWidget(new QLabel("基础页面内容", this));
    layout2->addWidget(new QLabel("应用页面内容", this));
    layout3->addWidget(new QLabel("热键页面内容", this));

    finalizeSetup();  // 检查并显示第一个页面
}
