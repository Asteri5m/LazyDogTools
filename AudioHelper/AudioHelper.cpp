#include "AudioHelper.h"

AudioHelper::AudioHelper(QWidget *parent)
    : ToolWidgetModel{parent}
{
    resize(800, 480);
    setMinimumSize(800, 480);

    // 使用模版样式, 如果不使用直接添加Tab，程序会崩溃
    setDefaultStyle();

    page1 = new QWidget(this);
    page2 = new QWidget(this);

    addTab(page1, QIcon(":/ico/todo.svg"), "任务");
    addTab(page2, QIcon(":/ico/user-settings.svg"), "偏好");

    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    QVBoxLayout *layout2 = new QVBoxLayout(page2);

    layout1->addWidget(new QLabel("任务页面内容", this));
    layout2->addWidget(new QLabel("偏好页面内容", this));

    finalizeSetup();  // 检查并显示第一个页面
    qDebug() << "实例化了 AudioHelper";
}
