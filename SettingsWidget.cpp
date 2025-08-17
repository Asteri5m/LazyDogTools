/**
 * @file SettingsWidget.cpp
 * @author Asteri5m
 * @date 2025-02-07 20:30:58
 * @brief Settings的交互窗口
 */

#include "LogHandler.h"
#include "SettingsWidget.h"
#include "Custom.h"
#include "TrayManager.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QTextBrowser>

SettingsWidget::SettingsWidget(Settings *settings, QWidget *parent)
    : ToolWidgetModel{parent}
    , mSettings(settings)
{
    setFixedSize(630, 425);
    // 取消其他按钮，只保留关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setWindowIcon(QIcon(":/ico/settings2.svg"));
    setWindowTitle("首选项");

    // 使用默认模板样式
    setDefaultStyle();

    mBasePage   = new QWidget(this);
    mAppPage    = new QWidget(this);
    mHotkeyPage = new QWidget(this);
    mAboutPage  = new QWidget(this);

    addTab(mAppPage,    QIcon(":/ico/apps.svg"), "应用");
    addTab(mBasePage,   QIcon(":/ico/settings.svg"), "基础");
    addTab(mHotkeyPage, QIcon(":/ico/keyboard.svg"), "热键");
    addTab(mAboutPage,  QIcon(":/ico/at.svg"), "关于");

    initBasePage();
    initAppPage();
    initHotkeyPage();
    initAboutPage();

    // 检查并显示第一个页面
    finalizeSetup();
}

SettingsWidget::~SettingsWidget()
{
    mBasePage->deleteLater();
    mAppPage->deleteLater();
    mHotkeyPage->deleteLater();
}

// 初始化"基础"页面
void SettingsWidget::initBasePage()
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

    startupLayout->addWidget(startCheckBox,      0, 0);
    startupLayout->addWidget(adminStartCheckBox, 0, 1);


    // 创建更新区域
    NoBorderGroupBox *updateGroupBox = new NoBorderGroupBox("更新");
    QGridLayout *updateLayout = new QGridLayout(updateGroupBox);

    MacStyleCheckBox *updateCheckBox = new MacStyleCheckBox("自动更新");
    MacStyleButton   *checkNewButton = new MacStyleButton("检查更新");

    updateLayout->addWidget(updateCheckBox, 0, 0);
    updateLayout->addWidget(checkNewButton, 0, 1);
    updateLayout->setColumnStretch(2, 1);   // 添加填充


    // 创建日志区域
    NoBorderGroupBox *logGroupBox = new NoBorderGroupBox("日志");
    QGridLayout *logLayout = new QGridLayout(logGroupBox);

    MacStyleCheckBox *debugCheckBox   = new MacStyleCheckBox("debug日志");
    MacStyleButton   *exportLogButton = new MacStyleButton("查看日志");

    logLayout->addWidget(debugCheckBox,   0, 0);
    logLayout->addWidget(exportLogButton, 0, 1);
    logLayout->setColumnStretch(2, 1); // 设置第 3 列的弹簧


    // 添加各个区域到mainLayout
    mainLayout->addWidget(startupGroupBox);
    mainLayout->addWidget(updateGroupBox);
    mainLayout->addWidget(logGroupBox);

    // 添加一个弹簧，用于撑起空白区域
    mainLayout->addStretch();

    // 加载配置
    loadConfigHandler(startCheckBox);
    loadConfigHandler(adminStartCheckBox);
    loadConfigHandler(updateCheckBox);
    loadConfigHandler(debugCheckBox);

    // 连接槽 - 按钮
    connect(checkNewButton,  SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(exportLogButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    // 连接槽 - 选择框
    connect(startCheckBox,      SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(adminStartCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(updateCheckBox,     SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(debugCheckBox,      SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
}

// 初始化"应用"页面
void SettingsWidget::initAppPage()
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

    // 绘制内容
    ToolManager& toolManager = ToolManager::instance();
    const ToolInfoMap& allToolsInfo = toolManager.getAllTools();
    for (auto it = allToolsInfo.begin(); it != allToolsInfo.end(); ++it)
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

        // 设置图标&应用名
        QIcon *icon = new QIcon(it->IconPath);
        QLabel *iconLabel = new QLabel();
        iconLabel->setPixmap(icon->pixmap(20, 20));
        iconLabel->setAlignment(Qt::AlignCenter);
        nameLayout->addWidget(iconLabel);
        nameLayout->addWidget(new QLabel(it->Name), 4);

        // 添加开关按钮
        MacSwitchButton *enableButton = new MacSwitchButton("enable:" + it->Name);
        enableLayout->addWidget(enableButton);
        loadConfigHandler(enableButton);
        connect(enableButton, SIGNAL(checkedChanged(bool)), this, SLOT(switchButtonChanged(bool)));

        // // 添加跳转按钮
        JumpButton *jumpButton = new JumpButton("jump:" + it->Name);
        jumpLayout->addWidget(jumpButton);
        connect(jumpButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

        appListLayout->addLayout(appItemLazyout);
    }

    appListLayout->addStretch();
    mainLayout->addWidget(appListArea);
}

// 初始化"快捷键"页面
void SettingsWidget::initHotkeyPage()
{
    // 使用滑动区域，内容过多时可以滑动
    QVBoxLayout *layout = new QVBoxLayout(mHotkeyPage);
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    QWidget *containerWidget = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    layout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(20, 10, 10, 10);

    // 添加热键编辑区域
    ToolManager& toolManager = ToolManager::instance();
    const ToolInfoMap& allToolsInfo = toolManager.getAllTools();
    for (auto it = allToolsInfo.begin(); it != allToolsInfo.end(); ++it)
    {
        auto hotKeyList = it->HotkeyList;   // 只有名，无值

        if (hotKeyList.isEmpty())
            continue;

        NoBorderGroupBox *toolGroupBox = new NoBorderGroupBox(it->Name);
        QGridLayout *toolLayout = new QGridLayout(toolGroupBox);

        for (int index=0; index<hotKeyList.length(); index++)
        {
            // 两列排布，QLabel+keyEdit+间距+QLabel+keyEdit
            int row     = index / 2;
            int column  = (index % 2) * 3;
            auto hotkeyName = hotKeyList.at(index);

            // 名字用应用名+热键名，形成唯一值
            CustomKeySequenceEdit *keyEdit = new CustomKeySequenceEdit("hotkey:" + it->Name + ":" + hotkeyName);
            toolLayout->addWidget(new QLabel(hotkeyName), row, column);
            toolLayout->addWidget(keyEdit, row, column + 1);

            // 先加载配置后绑定槽
            if (it->enabled)
                loadConfigHandler(keyEdit);
            connect(keyEdit, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(keySequenceChanged(QKeySequence)));
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

void SettingsWidget::initAboutPage()
{
    QGridLayout *mainLayout = new QGridLayout(mAboutPage);

    // 上半部分：版本相关信息
    QWidget *headWidget = new QWidget();
    mainLayout->addWidget(headWidget, 0, 0);

    // 添加富文本区域
    QWidget *richWidget = new QWidget();
    mainLayout->addWidget(richWidget, 1, 0);


    // 上半
    QGridLayout *headLayout = new QGridLayout(headWidget);
    headLayout->setContentsMargins(5, 5, 5, 5);
    // 添加图标
    QIcon icon(":/ico/LD.ico");
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(icon.pixmap(32, 32));
    iconLabel->setAlignment(Qt::AlignCenter);

    // 名称
    QLabel *nameLabel = new QLabel("LazyDogTools");
    QFont font = nameLabel->font();
    font.setPointSize(14);
    nameLabel->setFont(font);

    // 创建水平布局
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(iconLabel);
    hLayout->addWidget(nameLabel);
    hLayout->addStretch(1);
    hLayout->setContentsMargins(0,0,0,0);

    // 图标+名称 第一行，
    QWidget *container = new QWidget;
    container->setLayout(hLayout);
    headLayout->addWidget(container, 0, 0);

    // 版本信息 第二行
    MacStyleButton *checkNewButton = new MacStyleButton("更新历史");
    QLabel *versionLabel = new QLabel(QString("版本 %1 (%2)").arg(CURRENT_VERSION).arg(BUILD_DATE));
    headLayout->addWidget(versionLabel, 1, 0);
    headLayout->addWidget(checkNewButton, 1, 2);

    // 版权信息 第三行
    QString lastYear = BUILD_DATE.split('.').first();
    MacStyleButton *licenseButton = new MacStyleButton("开源协议");
    headLayout->addWidget(new QLabel(QString("版权所有 © 2024-%1 Asteri5m").arg(lastYear)), 2, 0);
    headLayout->addWidget(licenseButton, 2, 2);
    headLayout->setColumnStretch(1, 1);

    // 连接槽 - 按钮
    connect(checkNewButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(licenseButton,  SIGNAL(clicked()), this, SLOT(buttonClicked()));


    // 下半
    // 设置平滑滚动
    QVBoxLayout *richWidgetLayout = new QVBoxLayout(richWidget);
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    QWidget *containerWidget = new QWidget(scrollArea);
    QVBoxLayout *richLayout = new QVBoxLayout(containerWidget);
    richWidgetLayout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);

    // 设置样式，圆角-白底
    richLayout->setContentsMargins(0, 0, 0, 0);
    richWidgetLayout->setContentsMargins(10, 10, 0, 10);
    containerWidget->setObjectName("containerWidget");
    containerWidget->setStyleSheet(
        "QWidget#containerWidget {"
        "   background-color: #FCFCFC;"
        "}");
    richWidget->setObjectName("richWidget");
    richWidget->setStyleSheet(
        "QWidget#richWidget {"
        "   border-radius: 6px;"
        "   border: 1px solid #F0F0F0;"
        "   background-color: #FCFCFC;"
        "}");

    QLabel *label = new QLabel(this);
    richLayout->addWidget(label);
    label->setWordWrap(true); // 开启自动换行
    label->setTextFormat(Qt::RichText); // 开启富文本
    label->setTextInteractionFlags(Qt::TextBrowserInteraction); // 支持点击链接
    label->setOpenExternalLinks(true); // 点击超链接用浏览器打开
    QFont richFont("Microsoft YaHei", 10); // 字体名 + 字号，可调整
    label->setFont(richFont);

    // 加载数据
    QFile file(":/text/about.html");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString html = QString::fromUtf8(file.readAll());
        label->setText(html);
        file.close();
    } else {
        label->setText(R"(
            <p align="center">
             <strong>程序出现错误，读取配置信息失败！</strong>
             <a href='https://github.com/Asteri5m/LazyDogTools/issues' style='color:#2d8cff;'>反馈问题</a>
            </p>)");
        qCritical() << "程序出现错误，读取配置信息失败:" << file.fileName();
    }
}

// 打开特定应用
void SettingsWidget::jumpTool(QString toolName)
{
    ToolModel* tool = ToolManager::instance().getCreatedTool(toolName);
    if (tool != nullptr)
        tool->showWindow();
}

void SettingsWidget::showRichText(const QString &title, const QString &fileName)
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(title);
    dialog->resize(640, 400);
    dialog->setFixedWidth(640);

    // 主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    // QTextBrowser 显示富文本
    QTextBrowser *textBrowser = new QTextBrowser;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        textBrowser->setHtml(file.readAll());
        file.close();
    } else {
        textBrowser->setHtml(R"(
            <p align="center">
             <strong>程序出现错误，读取配置信息失败！</strong>
             <a href='https://github.com/Asteri5m/LazyDogTools/issues' style='color:#2d8cff;'>反馈问题</a>
            </p>)");
        qCritical() << "程序出现错误，读取配置信息失败:" << file.fileName();
    }
    mainLayout->addWidget(textBrowser); // 占用上方空间

    // 底部水平布局放按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(); // 弹性空白推到右边
    MacStyleButton *okButton = new MacStyleButton("确认");
    okButton->setNormalColorBlue(true);
    QObject::connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);

    mainLayout->addLayout(buttonLayout);
    dialog->exec();
    dialog->deleteLater();
}

// 对所有的按钮点击事件进行处理
void SettingsWidget::buttonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    qDebug() << "点击按钮: " << button->text();

    if (button->text().startsWith("jump:"))
        return jumpTool(button->text().split(":")[1]);

    if (button->text() == "查看日志")
    {
        QUrl fileUrl = QUrl::fromLocalFile("log/log.txt");
        // 使用默认程序打开日志文件
        if (!QDesktopServices::openUrl(fileUrl))
        {
            qWarning() << "Failed to open log file with default application.";
            showMessage(this, "程序出错了",
                        "打开文件失败了，程序遇到了一些问题……您可以：\n\n"
                        "重启程序或等待一段时间后重试；\n"
                        "手动打开文件：位于安装目录log文件夹下。\n",
                        MessageType::Critical);
        }
    } else if (button->text() == "检查更新")
    {
        qInfo() << "检查更新...";
        mSettings->checkForUpdates();
    } else if (button->text() == "更新历史")
    {
        showRichText("更新历史", ":/text/update_log.html");
    } else if (button->text() == "开源协议")
    {
        showRichText("license", ":/text/license.txt");
    } else
        qWarning() << "未处理的按钮事件: " << button->text();

}

// 对所有的下拉列表的事件进行处理
void SettingsWidget::comboBoxChanged(const QString currentText)
{
    MacStyleComboBox *comboBox = qobject_cast<MacStyleComboBox *>(sender());
    qDebug() << comboBox->text() << "切换选项:" << comboBox->currentIndex() << currentText;
    mSettings->saveConfig(comboBox->text(), currentText);
}

// 对所有的选项框事件进行处理
void SettingsWidget::checkBoxChecked(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    qDebug() << (checked ? "勾选:" : "取消勾选:") << checkBox->text();

    if (checkBox->text() == "debug日志")
    {
        LogHandler::instance().setLogLevel(checked ? DebugLevel : InfoLevel);
        qInfo() << (checked ? "开启" : "关闭") << "debug日志";
    } else if(checkBox->text() == "开机自启动") {
        if (!UAC::setApplicationStartup(checked))
        {
            checkBox->setChecked(!checked);
            qWarning() << "设置开机自启动失败";
            TrayManager::instance().showMessage("程序出错了",
                        "设置开机自启动失败!\n"
                        "请以管理员权限运行程序；\n"
                        "或者手动将程序添加到启动项中。",
                        QSystemTrayIcon::Warning);
            return;
        }   
    }

    mSettings->saveConfig(checkBox->text(), checked ? "true" : "false");
}

// 对所有开关的事件进行处理
void SettingsWidget::switchButtonChanged(bool checked)
{
    MacSwitchButton *switchButton = qobject_cast<MacSwitchButton *>(sender());
    qDebug() << switchButton->text().mid(7) << "选中: " << checked;

    if (mSettings->saveConfig(switchButton->text(), checked ? "true" : "false"))
    {
        if (checked)
        {
            ToolManager::instance().enableTool(switchButton->text().mid(7));
            // 工具被启用时，注册其热键
            const ToolInfo& info = ToolManager::instance().getToolInfo(switchButton->text().mid(7));
            for (const QString& hotkeyName : info.HotkeyList)
            {
                QString configKey = "hotkey:" + info.Name + ":" + hotkeyName;
                QString keySequence = mSettings->loadConfig(configKey);
                if (!keySequence.isEmpty())
                    mSettings->registerHotkey(configKey, QKeySequence(keySequence));
            }
        }
        else
        {
            // 工具被禁用时，注销其热键
            const ToolInfo& info = ToolManager::instance().getToolInfo(switchButton->text().mid(7));
            for (const QString& hotkeyName : info.HotkeyList)
                mSettings->unregisterHotkey("hotkey:" + info.Name + ":" + hotkeyName);
            ToolManager::instance().disableTool(switchButton->text().mid(7));
        }
        emit toolActiveChanged();
    }
}

// 快捷键输入框的处理事件
void SettingsWidget::keySequenceChanged(QKeySequence keySequence)
{
    CustomKeySequenceEdit* keySequenceEdit = qobject_cast<CustomKeySequenceEdit *>(sender());
    QString name = keySequenceEdit->text();

    // 快捷键未变化时，不处理
    if (mSettings->loadConfig(name) == keySequence.toString())
        return;
    
    // 获取工具名称
    QStringList parts = name.split(":");
    if (parts.length() != 3) {
        qWarning() << "Invalid hotkey name format:" << name;
        return;
    }
    
    // 检查工具是否启用
    const ToolInfo& info = ToolManager::instance().getToolInfo(parts[1]);
    if (!info.enabled) {
        // 工具未启用，仅保存配置
        mSettings->saveConfig(name, keySequence.toString());
        return;
    }

    // 第一步：注销
    mSettings->unregisterHotkey(name);
    keySequenceEdit->setAlert(false);
    mSettings->saveConfig(name, "");

    // 快捷键为空时，不注册
    if (keySequence.isEmpty())
        return;

    // 第二步：注册
    if (!mSettings->registerHotkey(name, keySequence))
        keySequenceEdit->setAlert(true, "快捷键被占用，请重新设置");
    else
        keySequenceEdit->setAlert(false);
    mSettings->saveConfig(name, keySequence.toString());
}

// 加载配置
template<typename T>
void SettingsWidget::loadConfigHandler(T *widget)
{
    QString value;
    QString typeName = QString(typeid(widget).name()).split(' ')[1];

    if (typeName == "MacStyleCheckBox") {
        MacStyleCheckBox* checkbox = qobject_cast<MacStyleCheckBox*>(widget);
        if (checkbox) {
            value = mSettings->loadConfig(checkbox->text());
            checkbox->setChecked(value == "true" ? true : false);
            return;
        }
    } else if (typeName == "MacSwitchButton") {
        MacSwitchButton* switchButton = qobject_cast<MacSwitchButton*>(widget);
        if (switchButton) {
            value = mSettings->loadConfig(switchButton->text());
            switchButton->setChecked(value == "true" ? true : false);
            return;
        }
    } else if (typeName == "MacStyleComboBox") {
        MacStyleComboBox* comboBox = qobject_cast<MacStyleComboBox*>(widget);
        if (comboBox) {
            value = mSettings->loadConfig(comboBox->text());
            comboBox->setCurrentText(value);
            return;
        }
    } else if (typeName == "CustomKeySequenceEdit") {
        CustomKeySequenceEdit* keyEdit = qobject_cast<CustomKeySequenceEdit*>(widget);
        if (keyEdit) {
            QString name(keyEdit->text());
            value = mSettings->loadConfig(name);
            QKeySequence keySequence(value);
            if (!keySequence.isEmpty())
                keyEdit->setKeySequence(QKeySequence(value));

            if (mSettings->queryHotkeyState(name) || keySequence.isEmpty()
                || !ToolManager::instance().getToolInfo(name.split(":")[1]).enabled)
                keyEdit->setAlert(false);
            else
                keyEdit->setAlert(true, "快捷键被占用，请重新设置");
            return;
        }
    }

    qCritical() << "无法识别的类型:" << typeName;
}
