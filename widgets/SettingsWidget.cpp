/**
 * @file SettingsWidget.cpp
 * @author Asteri5m
 * @date 2025-02-07 20:30:58
 * @brief Settings的交互窗口
 */


#include "utils/Custom.h"
#include "utils/Constants.h"
#include "managers/LogHandler.h"
#include "managers/TrayManager.h"
#include "managers/ThemeManager.h"
#include "widgets/SmoothScrollArea.h"
#include "components/CustomKeySequenceEdit.h"
#include "SettingsWidget.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QTextBrowser>
#include <QSlider>
#include <QLabel>

SettingsWidget::SettingsWidget(Settings *settings, QWidget *parent)
    : ToolWidgetModel{parent}
    , mSettings(settings)
{
    setFixedSize(630, 425);
    // 取消其他按钮，只保留关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    setWindowIcon(QIcon(":/ico/settings2.svg"));
    setWindowTitle("首选项");

    // 初始化亚克力效果设置
    ThemeManager &themeManager = ThemeManager::instance();
    setAcrylicEnabled(themeManager.acrylicEnabled());
    setAcrylicOpacity(themeManager.acrylicOpacity());

    // 使用默认模板样式
    setDefaultStyle();

    mBasePage   = new TransparentWidget(this);
    mAppPage    = new TransparentWidget(this);
    mHotkeyPage = new TransparentWidget(this);
    mAboutPage  = new TransparentWidget(this);
    mThemePage  = new TransparentWidget(this);

    addTab(mAppPage,    ":/ico/apps", "应用");
    addTab(mBasePage,   ":/ico/settings", "基础");
    addTab(mThemePage,  ":/ico/palette", "主题");
    addTab(mHotkeyPage, ":/ico/keyboard", "热键");
    addTab(mAboutPage,  ":/ico/at", "关于");

    initBasePage();
    initAppPage();
    initThemePage();
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
    QWidget *containerWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    layout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);

    layout->setContentsMargins(0, MARGIN_TINY, 0, MARGIN_TINY);
    mainLayout->setContentsMargins(MARGIN_TINY, 0, MARGIN_TINY, 0);
    mainLayout->setSpacing(MARGIN_LARGE);

    // 创建启动区域
    CustomGroupBox *startupGroupBox = new CustomGroupBox("启动");
    QGridLayout *startupLayout = new QGridLayout(startupGroupBox);

    MacStyleCheckBox *startCheckBox      = new MacStyleCheckBox("开机自启动");
    MacStyleCheckBox *adminStartCheckBox = new MacStyleCheckBox("管理员模式启动");

    startupLayout->addWidget(startCheckBox,      0, 0);
    startupLayout->addWidget(adminStartCheckBox, 0, 1);


    // 创建更新区域
    CustomGroupBox *updateGroupBox = new CustomGroupBox("更新");
    QGridLayout *updateLayout = new QGridLayout(updateGroupBox);

    MacStyleCheckBox *updateCheckBox = new MacStyleCheckBox("自动更新");
    MacStyleButton   *checkNewButton = new MacStyleButton("检查更新");
    checkNewButton->setNormalColorBlue(true);

    updateLayout->addWidget(updateCheckBox, 0, 0);
    updateLayout->addWidget(checkNewButton, 0, 2);
    updateLayout->setColumnStretch(1, 1);   // 设置第 2 列的弹簧


    // 创建日志区域
    CustomGroupBox *logGroupBox = new CustomGroupBox("日志");
    QGridLayout *logLayout = new QGridLayout(logGroupBox);

    MacStyleCheckBox *debugCheckBox   = new MacStyleCheckBox("debug日志");
    MacStyleButton   *exportLogButton = new MacStyleButton("查看日志");
    exportLogButton->setNormalColorBlue(true);

    logLayout->addWidget(debugCheckBox,   0, 0);
    logLayout->addWidget(exportLogButton, 0, 2);
    logLayout->setColumnStretch(1, 1); // 设置第 2 列的弹簧

#ifdef QT_DEBUG
    // 创建测试区域
    CustomGroupBox *testGroupBox = new CustomGroupBox("测试");
    QGridLayout *testLayout = new QGridLayout(testGroupBox);

    MacStyleButton *acrylicTestButton = new MacStyleButton("测试");
    acrylicTestButton->setNormalColorBlue(true);

    testLayout->addWidget(acrylicTestButton, 0, 0);
    testLayout->setColumnStretch(1, 1); // 设置第 2 列的弹簧

    connect(acrylicTestButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
#endif

    // 添加各个区域到mainLayout
    mainLayout->addWidget(startupGroupBox);
    mainLayout->addWidget(updateGroupBox);
    mainLayout->addWidget(logGroupBox);

#ifdef QT_DEBUG
    mainLayout->addWidget(testGroupBox);
#endif

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
    QVBoxLayout *layout = new QVBoxLayout(mAppPage);
    TransparentWidget *backWidget = new TransparentWidget();
    layout->setContentsMargins(MARGIN_TINY, MARGIN_TINY, MARGIN_TINY, MARGIN_TINY);
    layout->addWidget(backWidget);
    backWidget->setCornerRadius(RADIUS_XLARGE);
    backWidget->setBorderWidth(1);
    backWidget->setBorderColor(QColor(COLOR_BORDER));
    backWidget->setSolidColor(QColor(COLOR_BG_CARD));
    backWidget->setAcrylicColor(QColor(COLOR_BG_CARD));
    backWidget->setAcrylicAlpha(ThemeManager::instance().cardOpacity() * 255 / 100);
    connect(&ThemeManager::instance(), &ThemeManager::cardOpacityChanged, backWidget, &TransparentWidget::onAcrylicOpacityChanged);
    addDropShadowEffect(backWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(backWidget);
    mainLayout->setContentsMargins(5, 10, 5, 10);
    mainLayout->setSpacing(0);

    // 标题
    QHBoxLayout *titleLazyout = new QHBoxLayout();
    QLabel *nameLable   = new QLabel("应用");
    QLabel *enableLable = new QLabel("启用");
    QLabel *jumpLable   = new QLabel("跳转");

    // 使用样式表设置字体加粗并加大一号
    QString styleSheet = QString("QLabel { font-weight: %1; font-size: %2px;}").arg(QFont::Medium).arg(FONT_SIZE_XLARGE);
    nameLable  ->setStyleSheet(styleSheet);
    enableLable->setStyleSheet(styleSheet);
    jumpLable  ->setStyleSheet(styleSheet);

    titleLazyout->addWidget(nameLable,   4, Qt::AlignCenter);
    titleLazyout->addWidget(enableLable, 1, Qt::AlignCenter);
    titleLazyout->addWidget(jumpLable,   1, Qt::AlignCenter);
    mainLayout->addLayout(titleLazyout);


    // 添加横线---标题与内容的分割线
    mainLayout->addSpacerItem(new QSpacerItem(1, 5, QSizePolicy::Minimum, QSizePolicy::Minimum));
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border: none; background-color: " COLOR_SEPARATOR ";");
    line->setFixedHeight(2);
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

    // 设置字体
    applistWidget->setStyleSheet(QString("font-size: %1px; border: none;").arg(FONT_SIZE_XLARGE));

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

        // 添加间隔线
        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("border:none; background-color: " COLOR_SEPARATOR_DARK "; height: 1px;");
        line->setFixedHeight(1);
        appListLayout->addWidget(line);
    }

    appListLayout->addStretch();
    mainLayout->addWidget(appListArea);
}

// 初始化"主题"页面
void SettingsWidget::initThemePage()
{
    QVBoxLayout *layout = new QVBoxLayout(mThemePage);
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    QWidget *containerWidget = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    layout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(containerWidget);

    layout->setContentsMargins(0, MARGIN_TINY, 0, MARGIN_TINY);
    mainLayout->setContentsMargins(MARGIN_TINY, 0, MARGIN_TINY, 0);
    mainLayout->setSpacing(MARGIN_LARGE);

    ThemeManager &themeManager = ThemeManager::instance();

    // ===== 亚克力效果设置 =====
    CustomGroupBox *acrylicGroupBox = new CustomGroupBox("亚克力效果");
    QVBoxLayout *acrylicLayout = new QVBoxLayout(acrylicGroupBox);

    // 启用亚克力效果开关
    QHBoxLayout *acrylicSwitchLayout = new QHBoxLayout();
    MacStyleCheckBox *acrylicCheckBox = new MacStyleCheckBox("启用亚克力毛玻璃效果");
    acrylicSwitchLayout->addWidget(acrylicCheckBox);
    acrylicSwitchLayout->addStretch();
    acrylicLayout->addLayout(acrylicSwitchLayout);

    // 透明度滑块
    QHBoxLayout *opacityLayout = new QHBoxLayout();
    QLabel *opacityLabel = new QLabel("背景不透明度:");
    MacStyleSlider *opacitySlider = new MacStyleSlider();
    opacitySlider->setMinimum(0);
    opacitySlider->setMaximum(100);
    opacitySlider->setValue(themeManager.acrylicOpacity());
    opacitySlider->setFixedWidth(240);
    QLabel *opacityValueLabel = new QLabel(QString("%1%").arg(themeManager.acrylicOpacity()));
    opacityValueLabel->setFixedWidth(40);

    opacityLayout->addWidget(opacityLabel);
    opacityLayout->addWidget(opacitySlider);
    opacityLayout->addWidget(opacityValueLabel);
    opacityLayout->addStretch(1);
    acrylicLayout->addLayout(opacityLayout);

    // 卡片不透明度滑块
    QHBoxLayout *cardOpacityLayout = new QHBoxLayout();
    QLabel *cardOpacityLabel = new QLabel("卡片不透明度:");
    MacStyleSlider *cardOpacitySlider = new MacStyleSlider();
    cardOpacitySlider->setMinimum(0);
    cardOpacitySlider->setMaximum(100);
    cardOpacitySlider->setValue(themeManager.cardOpacity());
    cardOpacitySlider->setFixedWidth(240);
    QLabel *cardOpacityValueLabel = new QLabel(QString("%1%").arg(themeManager.cardOpacity()));
    cardOpacityValueLabel->setFixedWidth(40);

    cardOpacityLayout->addWidget(cardOpacityLabel);
    cardOpacityLayout->addWidget(cardOpacitySlider);
    cardOpacityLayout->addWidget(cardOpacityValueLabel);
    cardOpacityLayout->addStretch(1);
    acrylicLayout->addLayout(cardOpacityLayout);

    // 提示信息
    QLabel *acrylicHint = new QLabel("亚克力效果仅在 Windows 11 及以上版本可用");
    acrylicHint->setStyleSheet(QString("color: %1; font-size: %2px;").arg(COLOR_TEXT_SECONDARY).arg(FONT_SIZE_MEDIUM));
    acrylicLayout->addWidget(acrylicHint);

    mainLayout->addWidget(acrylicGroupBox);

    // ===== 配色方案设置 =====
    QVBoxLayout *colorSettingsLayout = new QVBoxLayout();
    colorSettingsLayout->setSpacing(10);

    CustomGroupBox *colorGroupBox = new CustomGroupBox("配色方案");
    QVBoxLayout *colorLayout = new QVBoxLayout(colorGroupBox);
    colorLayout->setSpacing(15);

    // 预设主题选择
    QHBoxLayout *presetLayout = new QHBoxLayout();
    QLabel *presetLabel = new QLabel("预设主题:");
    MacStyleComboBox *presetComboBox = new MacStyleComboBox("presetTheme");
    for (const auto &preset : themeManager.getPresets()) {
        presetComboBox->addItem(preset.name);
    }
    presetComboBox->setCurrentText(themeManager.themeName());
    presetLayout->addWidget(presetLabel);
    presetLayout->addWidget(presetComboBox);
    colorSettingsLayout->addLayout(presetLayout);

    // 主色调
    QHBoxLayout *primaryLayout = new QHBoxLayout();
    primaryLayout->setSpacing(10);
    QLabel *primaryLabel = new QLabel("主色调:");
    ColorPickerWidget *primaryColorPicker = new ColorPickerWidget(themeManager.primaryColor(), this);
    primaryLayout->addWidget(primaryLabel);
    primaryLayout->addWidget(primaryColorPicker);
    primaryLayout->addStretch();
    colorSettingsLayout->addLayout(primaryLayout);

    // 强调色
    QHBoxLayout *accentLayout = new QHBoxLayout();
    accentLayout->setSpacing(10);
    QLabel *accentLabel = new QLabel("强调色:");
    ColorPickerWidget *accentColorPicker = new ColorPickerWidget(themeManager.accentColor(), this);
    accentLayout->addWidget(accentLabel);
    accentLayout->addWidget(accentColorPicker);
    accentLayout->addStretch();
    colorSettingsLayout->addLayout(accentLayout);

    // 配色预览（垂直排列，整体靠右）
    QVBoxLayout *previewLayout = new QVBoxLayout();
    previewLayout->setSpacing(5);
    QLabel *previewTitle = new QLabel("配色预览:");
    QWidget *previewWidget = new QWidget();
    previewWidget->setStyleSheet(generatePreviewStyleSheet());
    previewLayout->addWidget(previewTitle);
    previewLayout->addWidget(previewWidget);
    previewLayout->setStretch(1, 1);

    // 水平排列：左侧颜色设置 + 右侧预览
    QHBoxLayout *colorRowLayout = new QHBoxLayout();
    colorRowLayout->setSpacing(35);
    colorRowLayout->addLayout(colorSettingsLayout);
    colorRowLayout->addLayout(previewLayout);
    colorRowLayout->setStretch(1, 1);

    colorLayout->addLayout(colorRowLayout);

    mainLayout->addWidget(colorGroupBox);

    // ===== 重置按钮 =====
    QHBoxLayout *resetLayout = new QHBoxLayout();
    MacStyleButton *resetButton = new MacStyleButton("恢复默认");
    resetButton->setNormalColorBlue(true);
    resetLayout->addStretch(1);
    resetLayout->addWidget(resetButton);
    mainLayout->addStretch(1);
    mainLayout->addLayout(resetLayout);


    // ===== 主题切换信号连接 =====
    // 使用 Qt::UniqueConnection 确保不会重复连接

    // 初始化控件状态
    acrylicCheckBox->setChecked(themeManager.acrylicEnabled());
    opacitySlider->setValue(themeManager.acrylicOpacity());
    opacityValueLabel->setText(QString("%1%").arg(themeManager.acrylicOpacity()));
    cardOpacitySlider->setValue(themeManager.cardOpacity());
    cardOpacityValueLabel->setText(QString("%1%").arg(themeManager.cardOpacity()));
    primaryColorPicker->setColor(themeManager.primaryColor());
    accentColorPicker->setColor(themeManager.accentColor());
    previewWidget->setStyleSheet(generatePreviewStyleSheet());

    // 亚克力开关
    connect(acrylicCheckBox, &MacStyleCheckBox::clicked, &themeManager, &ThemeManager::switchAcrylicEnabled, Qt::UniqueConnection);

    // 透明度滑块 - 实时预览，释放时保存
    connect(opacitySlider, &QSlider::valueChanged, &themeManager, &ThemeManager::switchAcrylicOpacity, Qt::UniqueConnection);
    connect(opacitySlider, &QSlider::valueChanged, this, [opacityValueLabel](int value) {
        opacityValueLabel->setText(QString("%1%").arg(value));
    });
    connect(opacitySlider, &QSlider::sliderReleased, &themeManager, &ThemeManager::saveSettings, Qt::UniqueConnection);

    // 卡片不透明度滑块 - 实时预览，释放时保存
    connect(cardOpacitySlider, &QSlider::valueChanged, &themeManager, &ThemeManager::switchCardOpacity, Qt::UniqueConnection);
    connect(cardOpacitySlider, &QSlider::valueChanged, this, [cardOpacityValueLabel](int value) {
        cardOpacityValueLabel->setText(QString("%1%").arg(value));
    });
    connect(cardOpacitySlider, &QSlider::sliderReleased, &themeManager, &ThemeManager::saveSettings, Qt::UniqueConnection);

    // 预设主题选择
    connect(presetComboBox, &MacStyleComboBox::currentIndexChanged, &themeManager,
            static_cast<void(ThemeManager::*)(int)>(&ThemeManager::switchToPreset), Qt::UniqueConnection);

    // 主色调选择器
    connect(primaryColorPicker, &ColorPickerWidget::colorChanged, &themeManager, &ThemeManager::switchPrimaryColor, Qt::UniqueConnection);
    connect(primaryColorPicker, &ColorPickerWidget::colorChanged, this, [presetComboBox](const QColor &) {
        presetComboBox->setCurrentText("自定义");
    });

    // 强调色选择器
    connect(accentColorPicker, &ColorPickerWidget::colorChanged, &themeManager, &ThemeManager::switchAccentColor, Qt::UniqueConnection);
    connect(accentColorPicker, &ColorPickerWidget::colorChanged, this, [presetComboBox](const QColor &) {
        presetComboBox->setCurrentText("自定义");
    });

    // 重置按钮
    connect(resetButton, &QPushButton::clicked, []() {
        ThemeManager::instance().switchToPreset("默认");
        ThemeManager::instance().switchAcrylicEnabled(ACRYLIC_ENABLED_DEFAULT);
        ThemeManager::instance().switchAcrylicOpacity(OPACITY_BG_DEFAULT);
        ThemeManager::instance().switchCardOpacity(OPACITY_CARD_DEFAULT);
    });

    // 主题整体变化时更新UI
    connect(&themeManager, &ThemeManager::themeChanged, this, [this, acrylicCheckBox, opacitySlider, cardOpacitySlider, presetComboBox, primaryColorPicker, accentColorPicker, previewWidget, opacityValueLabel, cardOpacityValueLabel]() {
        acrylicCheckBox->setChecked(ThemeManager::instance().acrylicEnabled());
        opacitySlider->setValue(ThemeManager::instance().acrylicOpacity());
        opacityValueLabel->setText(QString("%1%").arg(ThemeManager::instance().acrylicOpacity()));
        cardOpacitySlider->setValue(ThemeManager::instance().cardOpacity());
        cardOpacityValueLabel->setText(QString("%1%").arg(ThemeManager::instance().cardOpacity()));
        presetComboBox->setCurrentText(ThemeManager::instance().themeName());
        primaryColorPicker->setColor(ThemeManager::instance().primaryColor());
        accentColorPicker->setColor(ThemeManager::instance().accentColor());
        previewWidget->setStyleSheet(generatePreviewStyleSheet());
    });

    // 颜色变化时更新预览
    connect(&themeManager, &ThemeManager::primaryColorChanged, this, [this, previewWidget]() {
        previewWidget->setStyleSheet(generatePreviewStyleSheet());
    });
    connect(&themeManager, &ThemeManager::accentColorChanged, this, [this, previewWidget]() {
        previewWidget->setStyleSheet(generatePreviewStyleSheet());
    });
}

// 辅助函数：生成预览样式表
QString SettingsWidget::generatePreviewStyleSheet() const
{
    return QString(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "        stop:0 %1, stop:0.5 %2, stop:1 %3);"
        "    border-radius: %4px;"
        "    border: 1px solid %5;"
        "}"
    ).arg(ThemeManager::instance().primaryColor().name())
     .arg(ThemeManager::instance().accentColor().name())
     .arg(ThemeManager::instance().primaryColor().lighter(120).name())
     .arg(RADIUS_LARGE).arg(COLOR_BORDER_DARK);
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

    layout->setContentsMargins(0, MARGIN_TINY, 0, MARGIN_TINY);
    mainLayout->setContentsMargins(MARGIN_TINY, 0, MARGIN_TINY, 0);
    mainLayout->setSpacing(MARGIN_LARGE);

    // 添加热键编辑区域
    ToolManager& toolManager = ToolManager::instance();
    const ToolInfoMap& allToolsInfo = toolManager.getAllTools();
    for (auto it = allToolsInfo.begin(); it != allToolsInfo.end(); ++it)
    {
        auto hotKeyList = it->HotkeyList;   // 只有名，无值

        if (hotKeyList.isEmpty())
            continue;

        CustomGroupBox *toolGroupBox = new CustomGroupBox(it->Name);
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

// 初始化"关于"页面
void SettingsWidget::initAboutPage()
{
    QGridLayout *mainLayout = new QGridLayout(mAboutPage);
    mainLayout->setContentsMargins(MARGIN_TINY, MARGIN_TINY, MARGIN_TINY, MARGIN_TINY);

    // 上半部分：版本相关信息
    QWidget *headWidget = new QWidget();
    mainLayout->addWidget(headWidget, 0, 0);

    // 添加富文本区域
    TransparentWidget *richWidget = new TransparentWidget();
    mainLayout->addWidget(richWidget, 1, 0);


    // 上半
    QGridLayout *headLayout = new QGridLayout(headWidget);
    headLayout->setContentsMargins(MARGIN_TINY, MARGIN_SMALL, MARGIN_TINY, MARGIN_SMALL);
    // 添加图标
    QIcon icon(":/ico/LD.ico");
    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(icon.pixmap(32, 32));
    iconLabel->setAlignment(Qt::AlignCenter);

    // 名称
    QLabel *nameLabel = new QLabel("LazyDogTools");
    QFont font = nameLabel->font();
    font.setPointSize(FONT_SIZE_XLARGE);
    nameLabel->setFont(font);

    // 创建水平布局
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(iconLabel);
    hLayout->addWidget(nameLabel);
    hLayout->addStretch(1);
    hLayout->setContentsMargins(0, 0, 0, 0);

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
    QString lastYear = QString(BUILD_DATE).split('.').first();
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
    QWidget *containerWidget = new QWidget();
    QVBoxLayout *richLayout = new QVBoxLayout(containerWidget);
    richWidgetLayout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);

    // 设置样式，圆角-白底
    richLayout->setContentsMargins(0, 0, 0, 0);
    richWidgetLayout->setContentsMargins(MARGIN_LARGE, MARGIN_LARGE, 2, MARGIN_LARGE);
    richWidget->setCornerRadius(RADIUS_XLARGE);
    richWidget->setBorderWidth(1);
    richWidget->setBorderColor(QColor(COLOR_BORDER));
    richWidget->setSolidColor(QColor(COLOR_BG_CARD));
    richWidget->setAcrylicColor(QColor(COLOR_BG_CARD));
    richWidget->setAcrylicAlpha(ThemeManager::instance().cardOpacity() * 255 / 100);
    connect(&ThemeManager::instance(), &ThemeManager::cardOpacityChanged, richWidget, &TransparentWidget::onAcrylicOpacityChanged);
    addDropShadowEffect(richWidget);

    QLabel *label = new QLabel(this);
    richLayout->addWidget(label);
    label->setWordWrap(true); // 开启自动换行
    label->setTextFormat(Qt::RichText); // 开启富文本
    label->setTextInteractionFlags(Qt::TextBrowserInteraction); // 支持点击链接
    label->setOpenExternalLinks(true); // 点击超链接用浏览器打开

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
    else
        qWarning() << "工具未启用,无法打开:" << toolName;
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
    // qDebug() << "点击按钮: " << button->text();

    if (button->text().startsWith("jump:"))
        return jumpTool(button->text().split(":")[1]);

#ifdef QT_DEBUG
    if (button->text() == "测试")
    {
        AcrylicWidget *w = new AcrylicWidget();
        w->show();
    }
#endif

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
