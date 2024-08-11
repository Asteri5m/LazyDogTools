#include "AudioHelperWedgit.h"

AudioHelperWedgit::AudioHelperWedgit(QWidget *parent)
    : ToolWidgetModel{parent}
{
    resize(630, 425);



    page1 = new QWidget(this);
    page2 = new QWidget(this);

    addTab(page1, QIcon(":/ico/LD.ico"), "任务");
    addTab(page2, QIcon(":/ico/LD.ico"), "偏好");

    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    QVBoxLayout *layout2 = new QVBoxLayout(page2);

    layout1->addWidget(new QLabel("任务页面内容", this));
    layout2->addWidget(new QLabel("偏好页面内容", this));

    finalizeSetup();  // 检查并显示第一个页面
}