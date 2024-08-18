#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include "Settings.h"

Settings::Settings(QWidget *parent)
    : ToolWidgetModel{parent}
{
    setFixedSize(630, 425);
    // 取消其他按钮，只保留关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    // 使用默认模板样式
    setDefaultStyle();

    mBasePage = new QWidget(this);
    mAppPage = new QWidget(this);
    mShortcutsPage = new QWidget(this);

    initBasePage();

    addTab(mBasePage, QIcon(":/ico/LD.ico"), "基础");
    addTab(mAppPage, QIcon(":/ico/LD.ico"), "应用");
    addTab(mShortcutsPage, QIcon(":/ico/LD.ico"), "热键");

    // QVBoxLayout *layout1 = new QVBoxLayout(mBasePage);
    QVBoxLayout *layout2 = new QVBoxLayout(mAppPage);
    QVBoxLayout *layout3 = new QVBoxLayout(mShortcutsPage);

    // layout1->addWidget(new QLabel("基础页面内容", this));
    layout2->addWidget(new QLabel("应用页面内容", this));
    layout3->addWidget(new QLabel("热键页面内容", this));

    layout2->setContentsMargins(0, 0, 0, 0);
    mAppPage->setContentsMargins(10, 10, 10, 10);
    mAppPage->setStyleSheet("background-color: #FFFFFF;");

    finalizeSetup();  // 检查并显示第一个页面
}

// 设置ToolManager列表，通过设置可以修改ToolManager的部分属性，例如是否启用
void Settings::setToolManagerList(ToolManagerList *toolManagerList)
{
    mToolManagerList = toolManagerList;
}


// 初始化“基础”页面
void Settings::initBasePage()
{
    // 使用滑动区域，内容过多时可以滑动
    QVBoxLayout *layout = new QVBoxLayout(mBasePage);
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    QWidget *containerWidget = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    layout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(20, 10, 10, 10);

    // 创建启动区域
    NoBorderGroupBox *startupGroupBox = new NoBorderGroupBox("启动");
    QGridLayout *startupLayout = new QGridLayout(startupGroupBox);

    QCheckBox *startCheckBox      = new QCheckBox("开机自启动");
    QCheckBox *adminStartCheckBox = new QCheckBox("管理员模式启动");
    QCheckBox *startHideCheckBox  = new QCheckBox("启动后自动隐藏");

    startupLayout->addWidget(startCheckBox,      0, 0);
    startupLayout->addWidget(adminStartCheckBox, 0, 1);
    startupLayout->addWidget(startHideCheckBox,  1, 0);


    // 创建更新区域
    NoBorderGroupBox *updateGroupBox = new NoBorderGroupBox("更新");
    QGridLayout *updateLayout = new QGridLayout(updateGroupBox);

    QCheckBox      *updateCheckBox = new QCheckBox("自动更新");
    MacStyleButton *checkNewButton = new MacStyleButton("检查更新");

    updateLayout->addWidget(updateCheckBox, 0, 0);
    updateLayout->addWidget(checkNewButton, 0, 1);
    updateLayout->setColumnStretch(2, 1);   // 添加填充


    // 创建最小化设置区域
    NoBorderGroupBox *minimizeGroupBox = new NoBorderGroupBox("最小化设置");
    QGridLayout *minimizeLayout = new QGridLayout(minimizeGroupBox);
    minimizeLayout->addWidget(new QLabel("程序最小化显示在："), 0, 0);

    // 最小化选项
    MacStyleComboBox *minimizeComBox = new MacStyleComboBox("最小化设置");
    minimizeComBox->addItem("托盘");
    minimizeComBox->addItem("任务栏");
    minimizeLayout->addWidget(minimizeComBox, 0, 1);
    minimizeLayout->setColumnStretch(0, 1);
    minimizeLayout->setColumnStretch(1, 1);
    minimizeLayout->setColumnStretch(2, 2);


    // 创建关闭设置区域
    NoBorderGroupBox *closeGroupBox = new NoBorderGroupBox("关闭设置");
    QGridLayout *closeLayout = new QGridLayout(closeGroupBox);
    closeLayout->addWidget(new QLabel("当点击关闭按钮后："), 0, 0);

    MacStyleComboBox *closeComBox = new MacStyleComboBox("关闭设置");
    closeComBox->addItem("最小化托盘");
    closeComBox->addItem("退出");
    closeLayout->addWidget(closeComBox, 0, 1);
    closeLayout->setColumnStretch(0, 1);
    closeLayout->setColumnStretch(1, 1);
    closeLayout->setColumnStretch(2, 2);


    // 创建日志区域
    NoBorderGroupBox *logGroupBox = new NoBorderGroupBox("日志");
    QGridLayout *logLayout = new QGridLayout(logGroupBox);

    QCheckBox      *debugCheckBox   = new QCheckBox("debug日志");
    MacStyleButton *exportLogButton = new MacStyleButton("导出日志");

    logLayout->addWidget(debugCheckBox,   0, 0);
    logLayout->addWidget(exportLogButton, 0, 1);
    logLayout->setColumnStretch(2, 1); // 设置第 3 列的弹簧


    // 添加各个区域到mainLayout
    mainLayout->addWidget(startupGroupBox);
    mainLayout->addWidget(updateGroupBox);
    mainLayout->addWidget(minimizeGroupBox);
    mainLayout->addWidget(closeGroupBox);
    mainLayout->addWidget(logGroupBox);

    // 添加一个弹簧，用于撑起空白区域
    mainLayout->addStretch();

    // 连接槽 - 按钮
    connect(checkNewButton,  SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(exportLogButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    // 连接槽 - 选择框
    connect(startCheckBox,      SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(adminStartCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(startHideCheckBox,  SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(updateCheckBox,     SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(debugCheckBox,      SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));

    // 连接槽 - 下拉框
    connect(minimizeComBox, SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxChanged(QString)));
    connect(closeComBox,    SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxChanged(QString)));
}

// 初始化“应用”页面
void Settings::initAppPage()
{

}

// 初始化“快捷键”页面
void Settings::initShortcutsPage()
{

}


// 对所有的按钮点击事件进行处理
void Settings::buttonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    qDebug() << "点击按钮：" << button->text();

}

// 对所有的下拉列表的事件进行处理
void Settings::comboBoxChanged(const QString currentText)
{
    MacStyleComboBox *comboBox = qobject_cast<MacStyleComboBox *>(sender());
    qDebug() << comboBox->getName() << "切换选项：" << comboBox->currentIndex() << currentText;

    // test : 测试能否完成应用的开关功能
    auto toolManager = mToolManagerList->at(1);
    toolManager->setActive(comboBox->currentIndex());
    toolManager->deleteUI();
    emit appActiveChanged();
    // test end
}

// 对所有的选项框事件进行处理
void Settings::checkBoxChecked(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    qDebug() << checkBox->text() << "选中：" << checked;

}


