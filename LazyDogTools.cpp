#include <QScrollArea>
#include "LazyDogTools.h"
#include "ui_lazydogtools.h"
#include "SettingsManager.h"
#include "AudioHelper/AudioHelperManager.h"
#include "TransHelper/TransHelperManager.h"

LazyDogTools::LazyDogTools(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LazyDogTools)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/ico/LD.ico"));

    // "设置"属于LazyDogTools的一部分，而不是应用"模块"
    SettingsManager* settingsManager = new SettingsManager();
    settingsManager->initialize();  // 手动初始化
    settingsManager->initUI();
    Settings* settings = settingsManager->getTool();

    // 设置中可以关闭不需要的工具，那么就需要更新主界面
    connect(settings, SIGNAL(appActiveChanged()), this, SLOT(updateUI()));

    mToolManagerList.append(settingsManager);
    mToolManagerList.append(new AudioHelperManager());
    mToolManagerList.append(new TransHelperManager());


    initUI();
    // 将应用管理器的列表传递给“设置”,完成settings剩余的初始化工作
    settings->setToolManagerList(&mToolManagerList);
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
    for(auto toolManager:mToolManagerList)
    {
        toolManager->initialize();
        if (toolManager->getActive())
        {
            toolManager->initUI();
            mMinToolList.append(MinToolListItem({
                id,
                toolManager->getIcon(),
                toolManager->getName(),
                toolManager->getDescription(),
                toolManager
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
    initData(); //初始化数据

    // 创建容器控件
    mWidget = new QWidget(this);
    mLayout = new QVBoxLayout(mWidget);

    for (const auto &minTool : mMinToolList)
    {
        MinToolWidget *widget = new MinToolWidget(minTool.id, minTool.icon, minTool.name, minTool.description);
        ToolManager* tool = minTool.tool;
        mLayout->addWidget(widget);
        connect(widget, SIGNAL(widgetDoubleClicked()), tool, SLOT(show()));
    }
    // 添加一个填充，以便于在Tool较少时撑起空间
    mLayout->addStretch();

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

    // 更新数据
    updateData();

    // 绘制新内容
    for (const auto &minTool : mMinToolList)
    {
        MinToolWidget *widget = new MinToolWidget(minTool.id, minTool.icon, minTool.name, minTool.description);
        ToolManager* tool = minTool.tool;
        mLayout->addWidget(widget);
        connect(widget, SIGNAL(widgetDoubleClicked()), tool, SLOT(show()));
    }
    // 添加一个垂直间隔，以便于在Tool较少时撑起空间
    mLayout->addStretch();
}

void LazyDogTools::updateData()
{
    mMinToolList.clear();
    short id = 0;
    for(auto toolManager:mToolManagerList)
    {
        if (toolManager->getActive())
        {
            toolManager->initUI();
            mMinToolList.append(MinToolListItem({
                id,
                toolManager->getIcon(),
                toolManager->getName(),
                toolManager->getDescription(),
                toolManager
            }));
            id++; // id 自增
        }
    }
}
