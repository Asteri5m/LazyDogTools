#include <QScrollArea>
#include "LazyDogTools.h"
#include "ui_lazydogtools.h"
#include "Settings.h"
#include "AudioHelper/AudioHelper.h"

LazyDogTools::LazyDogTools(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LazyDogTools)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/ico/LD.ico"));

    mToolList.append(new Settings());
    mToolList.append(new AudioHelper());
    AudioHelper *test = new AudioHelper();
    mToolList.append(test);

    initData();
    initUI();

    test->setActive(false);
    updateData();
    updateUI();
}

LazyDogTools::~LazyDogTools()
{
    delete ui;
}

/**
 * 初始化程序的数据结构，解析各应用的数据
*/
void LazyDogTools::initData()
{
    short id = 0;
    for(auto tool:mToolList)
    {
        tool->initialize();
        if (tool->getActive())
        {
            tool->initUI();
            mMinToolList.append(MinToolListItem({
                id,
                tool->getIcon(),
                tool->getName(),
                tool->getDescription(),
                tool
            }));
            id++; // id 自增
        }
    }
}

/**
 * 初始化UI
*/
void LazyDogTools::initUI()
{
    // 创建容器控件
    mWidget = new QWidget(this);
    mLayout = new QVBoxLayout(mWidget);

    for (const auto &minTool : mMinToolList)
    {
        MinToolWidget *widget = new MinToolWidget(minTool.id, minTool.icon, minTool.name, minTool.description);
        Tool* tool = minTool.tool;
        mLayout->addWidget(widget);
        connect(widget, SIGNAL(widgetDoubleClicked()), tool, SLOT(show()));
    }
    // 添加一个垂直间隔，以便于在Tool较少时撑起空间
    mLayout->addSpacerItem(new QSpacerItem(100, 100, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // 创建 QScrollArea 实例
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(mWidget);

    // 隐藏滚动条
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //布局设置
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(scrollArea);
    layout->setContentsMargins(0, 0, 0, 0); // 去除容器布局的边距

    ui->widget->setLayout(layout);
}

void LazyDogTools::updateUI()
{
    // 清空原来的内容
    QLayoutItem* item;
    while ((item = mLayout->takeAt(0)) != nullptr)
    {
        mLayout->removeItem(item);
        if (item->widget())
            delete item->widget();
        else
            delete item;
    }

    // 绘制新内容
    for (const auto &minTool : mMinToolList)
    {
        MinToolWidget *widget = new MinToolWidget(minTool.id, minTool.icon, minTool.name, minTool.description);
        Tool* tool = minTool.tool;
        mLayout->addWidget(widget);
        connect(widget, SIGNAL(widgetDoubleClicked()), tool, SLOT(show()));
    }
    // 添加一个垂直间隔，以便于在Tool较少时撑起空间
    mLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void LazyDogTools::updateData()
{
    mMinToolList.clear();
    short id = 0;
    for(auto tool:mToolList)
    {
        if (tool->getActive())
        {
            tool->initUI();
            mMinToolList.append(MinToolListItem({
                id,
                tool->getIcon(),
                tool->getName(),
                tool->getDescription(),
                tool
            }));
            id++; // id 自增
        }
    }
}
