// #include <QKeySequenceEdit>
#include "LogHandler.h"
#include "Settings.h"

Settings::Settings(QWidget *parent)
    : ToolWidgetModel{parent}
    , mHotkeyManager{new HotkeyManager(this)}
{
    setFixedSize(630, 425);
    // 取消其他按钮，只保留关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    // 使用默认模板样式
    setDefaultStyle();

    mBasePage      = new QWidget(this);
    mAppPage       = new QWidget(this);
    mShortcutsPage = new QWidget(this);

    initBasePage();

    addTab(mBasePage,      QIcon(":/ico/settings.svg"), "基础");
    addTab(mAppPage,       QIcon(":/ico/apps.svg"), "应用");
    addTab(mShortcutsPage, QIcon(":/ico/keyboard.svg"), "热键");

    // 检查并显示第一个页面
    finalizeSetup();

    // 安装全局的事件过滤器
    qApp->installNativeEventFilter(mHotkeyManager);

    // 连接热键按下信号
    connect(mHotkeyManager, SIGNAL(hotkeyPressed(int)), this, SLOT(onHotkeyPressed(int)));

    // LogHandler::instance().setLogLevel(DebugLevel);
    LogHandler::instance().setLogLevel(InfoLevel);
    LogHandler::instance().clearBuffer();
}

// 设置ToolManager列表，通过设置可以修改ToolManager的部分属性，例如是否启用
void Settings::setToolManagerList(ToolManagerList *toolManagerList)
{
    mToolManagerList = toolManagerList;

    // 这两个页面依赖于主页面传过来的数据
    initAppPage();
    initShortcutsPage();
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

    MacStyleCheckBox *startCheckBox      = new MacStyleCheckBox("开机自启动");
    MacStyleCheckBox *adminStartCheckBox = new MacStyleCheckBox("管理员模式启动");
    MacStyleCheckBox *startHideCheckBox  = new MacStyleCheckBox("启动后自动隐藏");

    startupLayout->addWidget(startCheckBox,      0, 0);
    startupLayout->addWidget(adminStartCheckBox, 0, 1);
    startupLayout->addWidget(startHideCheckBox,  1, 0);


    // 创建更新区域
    NoBorderGroupBox *updateGroupBox = new NoBorderGroupBox("更新");
    QGridLayout *updateLayout = new QGridLayout(updateGroupBox);

    MacStyleCheckBox *updateCheckBox = new MacStyleCheckBox("自动更新");
    MacStyleButton   *checkNewButton = new MacStyleButton("检查更新");

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

    MacStyleCheckBox *debugCheckBox   = new MacStyleCheckBox("debug日志");
    MacStyleButton   *exportLogButton = new MacStyleButton("导出日志");

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
    QVBoxLayout *mainLayout = new QVBoxLayout(mAppPage);
    mainLayout->setContentsMargins(0, 10, 0, 10);   // 取消左右边距
    mainLayout->setSpacing(0);


    // 标题
    QHBoxLayout *titleLazyout = new QHBoxLayout();
    QLabel *nameLable   = new QLabel("应用");
    QLabel *enableLable = new QLabel("启用");
    QLabel *jumpLable   = new QLabel("跳转");

    // 使用样式表设置字体加粗并加大一号
    nameLable  ->setStyleSheet("QLabel { font-weight: bold; font-size: 14px; }");
    enableLable->setStyleSheet("QLabel { font-weight: bold; font-size: 14px; }");
    jumpLable  ->setStyleSheet("QLabel { font-weight: bold; font-size: 14px; }");

    titleLazyout->addWidget(nameLable,   4, Qt::AlignCenter);
    titleLazyout->addWidget(enableLable, 1, Qt::AlignCenter);
    titleLazyout->addWidget(jumpLable,   1, Qt::AlignCenter);
    mainLayout->addLayout(titleLazyout);


    // 添加横线---标题与内容的分割线
    mainLayout->addSpacerItem(new QSpacerItem(1, 5, QSizePolicy::Minimum, QSizePolicy::Minimum));
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border:none; border-bottom: 1px solid gray");
    mainLayout->addWidget(line);


    // 内容区域，虽然大概率用不到，但还是用滚动组件包起来吧
    SmoothScrollArea *appListArea = new SmoothScrollArea();
    QWidget *applistWidget = new QWidget();
    QVBoxLayout *appListLayout = new QVBoxLayout(applistWidget);

    appListLayout->setContentsMargins(0, 0, 0, 0);
    appListArea->setContentsMargins(0, 0, 0, 0);
    appListLayout->setSpacing(0);
    appListArea->setWidgetResizable(true);
    appListArea->setWidget(applistWidget);

    // 设置字体、分割线
    applistWidget->setStyleSheet("font-weight: bold; font-size: 14px;"
                                 "border-bottom: 1px solid #DADADA;");

    // 隐藏滚动条
    appListArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    appListArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 绘制内容:从第二个开始，避免对“设置”进行操作
    for (int i=1; i<mToolManagerList->length(); i++)
    {
        QHBoxLayout *appItemLazyout = new QHBoxLayout();

        QWidget *nameWidget   = new QWidget();
        QWidget *enableWidget = new QWidget();
        QWidget *jumpWidget   = new QWidget();

        // 保持比例和标题一致
        appItemLazyout->addWidget(nameWidget,   4);
        appItemLazyout->addWidget(enableWidget, 1);
        appItemLazyout->addWidget(jumpWidget,   1);

        QHBoxLayout *nameLayout   = new QHBoxLayout(nameWidget);
        QHBoxLayout *enableLayout = new QHBoxLayout(enableWidget);
        QHBoxLayout *jumpLayout   = new QHBoxLayout(jumpWidget);

        auto toolManager = mToolManagerList->at(i);
        // 设置图标&应用名
        QIcon *icon = new QIcon(toolManager->getIcon());
        QLabel *iconLabel = new QLabel();
        iconLabel->setPixmap(icon->pixmap(20, 20));
        iconLabel->setAlignment(Qt::AlignCenter);
        nameLayout->addWidget(iconLabel);
        nameLayout->addWidget(new QLabel(toolManager->getName()), 4);

        // 添加开关按钮
        MacSwitchButton *enableButton = new MacSwitchButton(toolManager->getName());
        enableLayout->addWidget(enableButton);
        enableButton->setChecked(toolManager->getActive());
        connect(enableButton, SIGNAL(checkedChanged(bool)), this, SLOT(switchButtonChanged(bool)));

        // // 添加跳转按钮
        JumpButton *jumpButton = new JumpButton("jump:" + toolManager->getName());
        jumpLayout->addWidget(jumpButton);
        connect(jumpButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

        appListLayout->addLayout(appItemLazyout);
    }

    appListLayout->addStretch();
    mainLayout->addWidget(appListArea);

}

// 初始化“快捷键”页面
void Settings::initShortcutsPage()
{
    // 使用滑动区域，内容过多时可以滑动
    QVBoxLayout *layout = new QVBoxLayout(mShortcutsPage);
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    QWidget *containerWidget = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    layout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(20, 10, 10, 10);

    mHotkeyMap  = new HotkeyMap();
    mHotkeyIdMap = new HotkeyIdMap();

    // 添加热键编辑区域
    for (int i=1; i<mToolManagerList->length(); i++)
    {
        auto toolManager = mToolManagerList->at(i);
        auto hotKeyList = toolManager->getHotKey();

        if (hotKeyList->isEmpty())
            continue;

        NoBorderGroupBox *toolGroupBox = new NoBorderGroupBox(toolManager->getName());
        QGridLayout *toolLayout = new QGridLayout(toolGroupBox);

        for (int index=0; index<hotKeyList->length(); index++)
        {
            // 两列排布，QLabel+keyEdit+间距+QLabel+keyEdit
            int row     = index / 2;
            int column  = (index % 2) * 3;
            auto hotkey = hotKeyList->at(index);

            // 名字用应用名+热键名，形成唯一值
            CustomKeySequenceEdit *keyEdit = new CustomKeySequenceEdit(toolManager->getName() + ":" + hotkey.Name);
            toolLayout->addWidget(new QLabel(hotkey.Name), row, column);
            toolLayout->addWidget(keyEdit, row, column + 1);

            connect(keyEdit, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(keySequenceChanged(QKeySequence)));

            // 在绑定槽函数之后设置值，通过槽函数完成热键注册
            keyEdit->setKeySequence(hotkey.Shortkeys);
        }

        // 比较美观的间距
        toolLayout->setColumnStretch(0, 3);
        toolLayout->setColumnStretch(1, 8);
        toolLayout->setColumnStretch(2, 2);
        toolLayout->setColumnStretch(3, 3);
        toolLayout->setColumnStretch(4, 8);

        mainLayout->addWidget(toolGroupBox);
    }
    mainLayout->addStretch();
}

// 打开特定应用
void Settings::jumpTool(QString toolName)
{
    for (auto toolManager : *mToolManagerList)
    {
        if (toolName == toolManager->getName()
            && toolManager->getActive())
        {
            toolManager->show();
            return;
        }
    }
}

// 对所有的按钮点击事件进行处理
void Settings::buttonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    qDebug() << "点击按钮：" << button->text();

    if (button->text().startsWith("jump:"))
        jumpTool(button->text().split(":")[1]);

}

// 对所有的下拉列表的事件进行处理
void Settings::comboBoxChanged(const QString currentText)
{
    MacStyleComboBox *comboBox = qobject_cast<MacStyleComboBox *>(sender());
    qDebug() << comboBox->getName() << "切换选项：" << comboBox->currentIndex() << currentText;
}

// 对所有的选项框事件进行处理
void Settings::checkBoxChecked(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    qDebug() << checkBox->text() << "选中：" << checked;

    if (checkBox->text() == "debug日志") {
        LogHandler::instance().setLogLevel(checked ? DebugLevel : InfoLevel);
    }

}

// 对所有开关的事件进行处理
void Settings::switchButtonChanged(bool checked)
{
    MacSwitchButton *switchButton = qobject_cast<MacSwitchButton *>(sender());
    qDebug() << switchButton->text() << "选中：" << checked;

    for (auto toolManager : *mToolManagerList)
    {
        if (switchButton->text() == toolManager->getName())
        {
            toolManager->setActive(checked);
            emit appActiveChanged();
            return;
        }
    }
}

// 热键的处理事件
void Settings::onHotkeyPressed(int id)
{
    qDebug() << id << mHotkeyIdMap->value(id);

}

// 快捷键输入框的处理事件
void Settings::keySequenceChanged(QKeySequence keySequence)
{
    CustomKeySequenceEdit* keySequenceEdit = qobject_cast<CustomKeySequenceEdit *>(sender());
    QString name = keySequenceEdit->text();

    // 快捷键未变化时，不处理
    if (mHotkeyMap->value(name) == keySequence)
        return;

    // 第一步：注销
    for (auto it = mHotkeyIdMap->begin(); it != mHotkeyIdMap->end(); ++it)
    {
        if (it.value() == name)
        {
            qInfo() << "注销快捷键：" << name << mHotkeyMap->value(name);
            qDebug() << "操作id：" << mHotkeyMap->value(name) << it.key();
            mHotkeyManager->unregisterHotkey(it.key());
            mHotkeyIdMap->remove(it.key());
            mHotkeyMap->remove(name);
            keySequenceEdit->setAlert(false); // 如果有提示，则清除
            break;
        }
    }

    // 快捷键为空时，不注册
    if (keySequence.isEmpty())
        return;

    // 第二步：注册，id自增
    for (int id=1; id <= mHotkeyMap->size()+1; id++)
    {
        if (!mHotkeyIdMap->contains(id)) // 找一个未使用的id
        {
            mHotkeyIdMap->insert(id, name);
            mHotkeyMap->insert(name, keySequence);
            qInfo() << "注册快捷键：" << name << keySequence;
            qDebug() << "操作id：" << keySequence << id;
            if (!mHotkeyManager->registerHotkey(id, keySequence))
                keySequenceEdit->setAlert(true, "快捷键被占用，请重新设置");
            else
                keySequenceEdit->setAlert(false);
            break;
        }
    }
}
