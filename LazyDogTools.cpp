#include "LazyDogTools.h"
#include "ui_lazydogtools.h"
#include "CustomUI.h"

#include <QScrollArea>


LazyDogTools::LazyDogTools(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LazyDogTools)
{
    ui->setupUi(this);

    this->minToolWidgetTest();

}

LazyDogTools::~LazyDogTools()
{
    delete ui;
}

void LazyDogTools::minToolWidgetTest()
{
    struct ListItem {
        int     id;
        QString iconPath;
        QString name;
        QString description;
    };

    QVector<ListItem> items = {
       {0,":/ico/LD.ico", "设置1", "对工具进行全局设置"},
       {1,":/ico/LD.ico", "翻译助手2", "有道词典助手是一个词典助手\n他可以帮助你高效的学习，复习英语单词"},
       {2,":/ico/LD.ico", "音频助手3", "音频助手音频助手音频助手\n音频助手音频助手音频助手"},
       {3,":/ico/LD.ico", "设置4", "对工具进行全局设置"},
       {4,":/ico/LD.ico", "翻译助手5", "有道词典助手是一个词典助手\n他可以帮助你高效的学习，复习英语单词英语单词英语单词"},
       {5,":/ico/LD.ico", "音频助手6", "音频助手音频助手音频助手\n音频助手音频助手音频助手"},
       {6,":/ico/LD.ico", "设置7", "对工具进行全局设置"},
       {7,":/ico/LD.ico", "翻译助手8", "有道词典助手是一个词典助手\n他可以帮助你高效的学习，复习英语单词"},
       {8,":/ico/LD.ico", "音频助手9", "音频助手音频助手音频助手\n音频助手音频助手音频助手"},
       {9,":/ico/LD.ico", "设置10", "对工具进行全局设置"},
       {10,":/ico/LD.ico", "翻译助手11", "有道词典助手是一个词典助手\n他可以帮助你高效的学习，复习英语单词"},
       {11,":/ico/LD.ico", "音频助手12", "音频助手音频助手音频助手\n音频助手音频助手音频助手"},
    };


    // 创建容器控件
    QWidget *containerWidget = new QWidget(this);
    // containerWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);
    // containerLayout->setContentsMargins(0, 0, 0, 0); // 去除容器布局的边距

    for (const ListItem &item : items) {
        MinToolWidget *widget = new MinToolWidget(item.id, item.iconPath, item.name, item.description);
        containerLayout->addWidget(widget);
    }

    // 创建 QScrollArea 实例
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(containerWidget);
    // scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 隐藏滚动条
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // scrollArea->setContentsMargins(0, 0, 0, 0);

    //布局设置
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(scrollArea);
    layout->setContentsMargins(0, 0, 0, 0); // 去除容器布局的边距

    ui->widget->setLayout(layout);
}
