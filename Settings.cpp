#include "Settings.h"

Settings::Settings(QWidget *parent)
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

    // 创建按钮
    QPushButton *applyButton = new MacStyleButton("应用");
    QPushButton *cancelButton = new MacStyleButton("取消");

    // 创建水平布局，并将按钮添加进去
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 10, 10);
    buttonLayout->addStretch(1); // 添加水平填充
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);

    // 将 QStackedWidget 下面的一行用 buttonLayout 占据
    mMainLayout->addLayout(buttonLayout, mMainLayout->rowCount(), 0, 1, -1);

    // 将 Button 的 clicked() 信号连接到槽
    connect(applyButton,  &QPushButton::clicked, this, &Settings::apply);
    connect(cancelButton, &QPushButton::clicked, this, &QWidget::close);

    finalizeSetup();  // 检查并显示第一个页面
}

void Settings::apply()
{
    qDebug() << "应用设置";
    close();
}
