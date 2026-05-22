/**
 * @file AudioHelperWidget.cpp
 * @author Asteri5m
 * @date 2025-02-08 0:07:54
 * @brief AudioHelper的交互窗口
 */

#include "AudioHelperWidget.h"
#include "managers/ToolManager.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "SelectionDialog.h"

AudioHelperWidget::AudioHelperWidget(RelatedList *relatedList, QMap<QString, QString> *config, AudioDatabase* database, QWidget *parent)
    : ToolWidgetModel{parent}
    , mHomePage(new TransparentWidget(this))
    , mPrefsPage(new TransparentWidget(this))
    , mTaskTab(new QTreeWidget())
    , mRelatedList(relatedList)
    , mConfig(config)
    , mDatabase(database)
{
    resize(800, 480);
    setMinimumSize(800, 480);

    setWindowIcon(QIcon(":/ico/audiohelper.svg"));
    setWindowTitle("音频助手");

    // 使用模版样式, 如果不使用直接添加Tab，程序会崩溃
    setDefaultStyle();

    addTab(mHomePage,  ":/ico/todo", "任务");
    addTab(mPrefsPage, ":/ico/user-settings", "偏好");

    initHomePage();
    initPrefsPage();

    connect(mTaskTab, &QTreeWidget::itemClicked, this, &AudioHelperWidget::onTaskTabItemClicked);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &AudioHelperWidget::onThemeChanged);

    onThemeChanged(); // 设置主题
    finalizeSetup();  // 检查并显示第一个页面
}

AudioHelperWidget::~AudioHelperWidget()
{
    delete mHomePage;
    delete mPrefsPage;
}

QString AudioHelperWidget::queryConfig(const QString &key)
{
    return mConfig->value(key, QString());
}

void AudioHelperWidget::initHomePage()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(mHomePage);
    QHBoxLayout *footLayout = new QHBoxLayout();

    // 表格的父窗口，用于轻松修改边框
    TransparentWidget *tabWidget = new TransparentWidget();
    QHBoxLayout *tabLayout = new QHBoxLayout(tabWidget);
    tabLayout->addWidget(mTaskTab);

    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(footLayout);

    mainLayout->setContentsMargins(MARGIN_TINY, MARGIN_TINY, MARGIN_TINY, MARGIN_TINY);
    tabLayout->setContentsMargins(MARGIN_TINY, MARGIN_TINY, MARGIN_TINY, MARGIN_TINY);
    tabWidget->setCornerRadius(RADIUS_XLARGE);
    tabWidget->setBorderWidth(1);
    tabWidget->setBorderColor(QColor(COLOR_BORDER));
    tabWidget->setSolidColor(QColor(COLOR_BG_CARD));
    tabWidget->setAcrylicColor(QColor(COLOR_BG_CARD));
    tabWidget->setAcrylicAlpha(ThemeManager::instance().cardOpacity() * 255 / 100);
    connect(&ThemeManager::instance(), &ThemeManager::cardOpacityChanged, tabWidget, &TransparentWidget::onAcrylicOpacityChanged);
    addDropShadowEffect(tabWidget);

    // 设置样式
    mTaskTab->setSelectionMode(QAbstractItemView::SingleSelection); // 单选
    mTaskTab->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选择整行
    mTaskTab->setEditTriggers(QAbstractItemView::NoEditTriggers);   // 不可编辑
    mTaskTab->header()->setDefaultAlignment(Qt::AlignCenter);       // 表头居中
    mTaskTab->header()->setSectionResizeMode(QHeaderView::Stretch); // 自适应列宽
    mTaskTab->header()->setSectionsMovable(false);  // 禁止拖动列

    mTaskTab->setFrameStyle(QFrame::NoFrame);       // 无边框
    mTaskTab->setRootIsDecorated(false);            // 去掉树的展开箭头
    mTaskTab->setAlternatingRowColors(false);       // 禁用交替行颜色（配合亚克力效果）
    mTaskTab->setFocusPolicy(Qt::NoFocus);          // 去除虚线框
    mTaskTab->setWordWrap(false);                   // 禁用换行

    // 设置透明背景以适配亚克力效果
    mTaskTab->setAttribute(Qt::WA_TranslucentBackground);
    mTaskTab->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    mTaskTab->setAutoFillBackground(false);
    mTaskTab->viewport()->setAutoFillBackground(false);

    // 设置列和表头
    mTaskTab->setColumnCount(3);
    mTaskTab->setHeaderLabels({"关联项", "关联类型", "关联设备"});
    mTaskTab->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    mTaskTab->setColumnWidth(1, TAG_DEFAULT_WIDTH);

    // 遍历数据列表
    for (int i = 0; i < mRelatedList->length(); i++) {
        const RelatedItem *relatedItem = &mRelatedList->at(i);

        // 创建一行（Tree Widget Item）
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(mTaskTab);

        // 第一列：图标 + 名称
        QFileIconProvider iconProvider;
        QIcon icon = iconProvider.icon(QFileInfo(relatedItem->taskInfo.path));
        treeItem->setText(0, relatedItem->taskInfo.name);
        treeItem->setIcon(0, icon);

        // 第二列：标签（用自定义 TagLabel 代替文字）
        QString tag = relatedItem->typeInfo.tag.isEmpty() ? relatedItem->typeInfo.type : relatedItem->typeInfo.tag;
        TagLabel *tagLabel = new TagLabel(tag.length() > 2 ? tag : QString(tag).insert(1, "   "));
        tagLabel->setFixedWidth(TAG_DEFAULT_WIDTH);
        tagLabel->setTheme(TagTheme.value(tag, TagLabel::Theme::Default));
        mTaskTab->setItemWidget(treeItem, 1, tagLabel);

        // 第三列：设备名
        treeItem->setText(2, relatedItem->audioDeviceInfo.name);

        // 把行添加到 TreeWidget
        mTaskTab->addTopLevelItem(treeItem);
    }


    // 添加底部按钮
    footLayout->setContentsMargins(5, 5, 5, 0);

    MacStyleButton *addButton = new MacStyleButton("添加");
    MacStyleButton *delButton = new MacStyleButton("删除");
    MacStyleButton *chgButton = new MacStyleButton("修改");
    mTagButton = new MacStyleButton("标记场景");
    mTagButton->setNormalColorBlue(true);

    footLayout->addWidget(new QLabel("关联项管理"));
    footLayout->addWidget(addButton);
    footLayout->addWidget(delButton);
    footLayout->addWidget(chgButton);
    footLayout->addStretch(1);
    footLayout->addWidget(mTagButton);

    connect(addButton,   SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(delButton,   SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(chgButton,   SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(mTagButton,  SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void AudioHelperWidget::initPrefsPage()
{
    // 使用滑动区域，内容过多时可以滑动
    QVBoxLayout *layout = new QVBoxLayout(mPrefsPage);
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    QWidget *containerWidget = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(containerWidget);
    layout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true); // 使内容区域可以自动调整大小
    scrollArea->setWidget(containerWidget);
    
    layout->setContentsMargins(0, MARGIN_TINY, 0, MARGIN_TINY);
    mainLayout->setContentsMargins(MARGIN_TINY, 0, MARGIN_TINY, 0);
    mainLayout->setSpacing(10);

    // 模式
    CustomGroupBox *modeGroupBox = new CustomGroupBox("模式");
    QGridLayout *modeLayout = new QGridLayout(modeGroupBox);
    modeLayout->addWidget(new QLabel("选择触发任务的算法模式："), 0, 0);

    // 模式选项
    MacStyleComboBox *modeComBox = new MacStyleComboBox("任务模式");
    modeComBox->addItem("进程模式");
    modeComBox->addItem("窗口模式");
    modeComBox->addItem("智能模式");
    modeLayout->addWidget(modeComBox, 0, 2);
    modeLayout->setColumnStretch(1, 1);


    // 场景识别
    CustomGroupBox *sceneGroupBox = new CustomGroupBox("场景");
    QGridLayout *sceneLayout = new QGridLayout(sceneGroupBox);
    sceneLayout->addWidget(new QLabel("多条关联规则命中时的场景选择："), 0, 0);

    // 场景选项
    MacStyleComboBox *sceneComBox = new MacStyleComboBox("场景识别");
    sceneComBox->addItem("游戏场景");
    sceneComBox->addItem("影音场景");
    sceneComBox->addItem("普通场景");
    sceneLayout->addWidget(sceneComBox, 0, 2);
    sceneLayout->setColumnStretch(1, 1);


    // 通知
    CustomGroupBox *notifyGroupBox = new CustomGroupBox("通知");
    QGridLayout *notifyLayout = new QGridLayout(notifyGroupBox);
    MacStyleCheckBox *notifyCheckBox = new MacStyleCheckBox("切换时通知");
    notifyLayout->addWidget(notifyCheckBox, 0, 0);


    // 过滤
    CustomGroupBox *filterGroupBox = new CustomGroupBox("过滤");
    QGridLayout *filterLayout = new QGridLayout(filterGroupBox);
    MacStyleCheckBox *filterProcessCheckBox    = new MacStyleCheckBox("过滤系统项");
    MacStyleCheckBox *filterRepetitionCheckBox = new MacStyleCheckBox("过滤重复项");
    filterLayout->addWidget(filterProcessCheckBox, 1, 0);
    filterLayout->addWidget(filterRepetitionCheckBox, 1, 3);

    // 提示按钮
    HintLabel *processHint    = new HintLabel("?", "在添加关联项时过滤系统应用的进程与窗口");
    HintLabel *RepetitionHint = new HintLabel("?", "在添加关联项时过滤同一可执行文件的重复进程");
    filterLayout->addWidget(processHint, 1, 1);
    filterLayout->addWidget(RepetitionHint, 1, 4);

    // 设置填充
    filterLayout->setColumnStretch(2, 1);
    filterLayout->setColumnStretch(5, 1);


    // 添加各个区域到mainLayout
    mainLayout->addWidget(modeGroupBox);
    mainLayout->addWidget(sceneGroupBox);
    mainLayout->addWidget(notifyGroupBox);
    mainLayout->addWidget(filterGroupBox);

    // 添加一个弹簧，用于撑起空白区域
    mainLayout->addStretch();


    // 加载配置
    loadConfigHandler(notifyCheckBox);
    loadConfigHandler(filterProcessCheckBox);
    loadConfigHandler(filterRepetitionCheckBox);
    loadConfigHandler(modeComBox);
    loadConfigHandler(sceneComBox);


    // 连接槽 - 选择框
    connect(notifyCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(filterProcessCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(filterRepetitionCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));

    // 连接槽 - 下拉框
    connect(modeComBox, SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxChanged(QString)));
    connect(sceneComBox,  SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxChanged(QString)));
}

void AudioHelperWidget::onTaskTabItemClicked(QTreeWidgetItem *item, int)
{
    // 获取被点击的行
    // int row = item->;

    // 获取该行第二列的 widget
    QWidget *widget = mTaskTab->itemWidget(item, 1);

    if (widget)
    {
        // 假设 widget 是 TagLabel 类型，可以转换为 TagLabel 类型
        TagLabel *tagLabel = qobject_cast<TagLabel *>(widget);
        if (tagLabel)
        {
            if (tagLabel->text() == "游戏" || tagLabel->text() == "影音")
                mTagButton->setText("取消标记");
            else
                mTagButton->setText("标记场景");
        }
    }
}

void AudioHelperWidget::addRelatedItem()
{
    SelectionDialog selectionDialog(this);

    // 先选任务触发项
    if (selectionDialog.exec() != QDialog::Accepted) {
        qDebug() << "任务触发项: 取消选择";
        return;
    }

    SelectionInfo *selectionInfo = selectionDialog.selectedOption();
    qDebug() << "任务触发项: " <<  selectionInfo->taskInfo.name << "|" << selectionInfo->taskInfo.path;

    // 空值校验
    if (selectionInfo->taskInfo.name.isEmpty() && selectionInfo->taskInfo.path.isEmpty()) {
        showMessage(this, "空选项", "未选择任何触发项，请重新选择！", MessageType::Warning);
        return addRelatedItem();
    }

    // 重复值校验
    RelatedList queryList;
    mDatabase->queryItems("taskPath", selectionInfo->taskInfo.path, &queryList);
    if (!queryList.isEmpty())
    {
        qWarning() << "任务已存在: Name:" << queryList.at(0).taskInfo.name << "; TypeInfo:" << queryList.at(0).typeInfo.type;
        if (showMessage(this, "重复添加", "该选项已经在任务列表中，无法重复添加！\n\n是否继续添加其它项?\n", MessageType::Warning, Qt::PlainText, "继续", "返回") == QDialog::Accepted)
            return addRelatedItem();
        return;
    }

    // 再选任务关联项
    AudioChoiceDialog choiceDialog(QString(), this);
    if (choiceDialog.exec() != QDialog::Accepted)
    {
        qDebug() << "任务关联项: 取消选择";
        return;
    }

    AudioDeviceInfo* deviceInfo = choiceDialog.selectedOption();
    qDebug() << "任务关联项: " <<  deviceInfo->name;

    // 结构化数据
    TypeInfo typeInfo{selectionInfo->type, ""};
    RelatedItem relatedItem{0, selectionInfo->taskInfo, typeInfo, *deviceInfo};

    // 添加到列表和数据库
    if (!mDatabase->insertItem(relatedItem))
    {
        showMessage(this, "关联任务失败", QString("数据存储时失败!\n详情:%1").arg(mDatabase->lastError()), MessageType::Critical);
        return;
    }

    mRelatedList->append(relatedItem);

    // 创建顶层节点
    QTreeWidgetItem *item = new QTreeWidgetItem(mTaskTab);

    // 第一列：图标+名称
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(QFileInfo(relatedItem.taskInfo.path));
    item->setText(0, relatedItem.taskInfo.name);
    item->setIcon(0, icon);
    // 第二列：标签
    QString tag = relatedItem.typeInfo.tag.isEmpty() ? relatedItem.typeInfo.type : relatedItem.typeInfo.tag;
    TagLabel *tagLabel = new TagLabel(tag.length() > 2 ? tag : QString(tag).insert(1, "   "));
    tagLabel->setFixedWidth(TAG_DEFAULT_WIDTH);
    tagLabel->setTheme(TagTheme.value(tag, TagLabel::Theme::Default));
    mTaskTab->setItemWidget(item, 1, tagLabel);
    // 第三列：设备名
    item->setText(2, relatedItem.audioDeviceInfo.name);
    // 添加到 QTreeWidget
    mTaskTab->addTopLevelItem(item);
}

void AudioHelperWidget::delRelatedItem()
{
    QTreeWidgetItem *item = mTaskTab->currentItem();
    if (!item)
        return;

    // 获取对应的 RelatedItem
    int row = mTaskTab->indexOfTopLevelItem(item);
    const RelatedItem *relatedItem = &mRelatedList->at(row);
    qInfo() << "delete item:" << relatedItem->taskInfo.name << "-" << relatedItem->audioDeviceInfo.name;

    // 删除数据库记录
    if (!mDatabase->deleteItem(relatedItem->id)) {
        qCritical() << "Failed to delete item:" << mDatabase->lastError();
        return;
    }

    // 删除数据和界面
    mRelatedList->removeAt(row);
    delete mTaskTab->takeTopLevelItem(row);  // 释放 QTreeWidgetItem
    mTaskTab->clearSelection();
    mTaskTab->setCurrentItem(nullptr);
}

void AudioHelperWidget::changeRelatedItem()
{
    QTreeWidgetItem *item = mTaskTab->currentItem();
    if (!item)
        return;

    // 弹出选择对话框
    AudioChoiceDialog choiceDialog(QString(), this);
    if (choiceDialog.exec() != QDialog::Accepted) {
        qInfo() << "任务" << item->text(0) << "更改关联项: 取消选择";
        return;
    }

    AudioDeviceInfo* deviceInfo = choiceDialog.selectedOption();
    qInfo() << "任务" << item->text(0) << "更改关联项: " << deviceInfo->name;

    // 更新数据
    int row = mTaskTab->indexOfTopLevelItem(item);
    RelatedItem &relatedItem = (*mRelatedList)[row];
    relatedItem.audioDeviceInfo = *deviceInfo;

    if (!mDatabase->updateItem(relatedItem)) {
        qCritical() << "Failed to update item:" << mDatabase->lastError();
        return;
    }

    // 修改 UI 显示为新的值
    item->setText(2, relatedItem.audioDeviceInfo.name);

    // 清除选择
    mTaskTab->clearSelection();
}

void AudioHelperWidget::setSceneTag(bool isAdd)
{
    QTreeWidgetItem *item = mTaskTab->currentItem();
    if (!item)
        return;

    int current_row = mTaskTab->indexOfTopLevelItem(item);
    RelatedItem &relatedItem = (*mRelatedList)[current_row];

    if (isAdd) {
        TagSwitchDialog tagSwitchDialog(this);
        if (tagSwitchDialog.exec() != QDialog::Accepted) {
            qDebug() << "场景关联: 取消选择";
            return;
        }
        relatedItem.typeInfo.tag = tagSwitchDialog.selectedOption();
    } else {
        relatedItem.typeInfo.tag = "";
    }

    // 保存到数据库
    if (!mDatabase->updateItem(relatedItem)) {
        qCritical() << "Failed to update item:" << mDatabase->lastError();
        return;
    }

    // 删除旧的 widget
    QWidget *oldWidget = mTaskTab->itemWidget(item, 1);
    if (oldWidget) {
        oldWidget->deleteLater();
    }

    // 创建新的 TagLabel 并设置到第二列
    QString tag = relatedItem.typeInfo.tag == "" ? relatedItem.typeInfo.type : relatedItem.typeInfo.tag;
    TagLabel *tagLabel = new TagLabel(tag.length() > 2 ? tag : QString(tag).insert(1, "   "));
    tagLabel->setFixedWidth(TAG_DEFAULT_WIDTH);
    tagLabel->setTheme(TagTheme.value(tag, TagLabel::Theme::Default));
    mTaskTab->setItemWidget(item, 1, tagLabel);

    qDebug() << "场景关联:" << relatedItem.taskInfo.name << "|" << relatedItem.typeInfo.tag;

    // 更新按钮文本
    mTagButton->setText(isAdd ? "取消标记" : "标记场景");
}

void AudioHelperWidget::buttonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    qDebug() << "点击按钮: " << button->text();

    if (button->text() == "添加")
        addRelatedItem();
    else if (button->text() == "删除")
        delRelatedItem();
    else if (button->text() == "修改")
        changeRelatedItem();
    else if (button->text() == "标记场景")
        setSceneTag(true);
    else if (button->text() == "取消标记")
        setSceneTag(false);
}

void AudioHelperWidget::checkBoxChecked(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    QString key = checkBox->text();
    QString value = checked ? "true" : "false";
    qDebug() << (checked ? "勾选:" : "取消勾选:") << key;
    emit configChanged(key, value);
}

void AudioHelperWidget::comboBoxChanged(QString currentText)
{
    MacStyleComboBox *comboBox = qobject_cast<MacStyleComboBox *>(sender());
    QString key = comboBox->text();
    qDebug() << key << "切换选项:" << comboBox->currentIndex() << currentText;
    emit configChanged(key, currentText);
}

template<typename T>
void AudioHelperWidget::loadConfigHandler(T *widget)
{
    QString key, value;
    QString typeName = QString(typeid(widget).name()).split(' ')[1];

    if (typeName == "MacStyleCheckBox") {
        MacStyleCheckBox* checkbox = qobject_cast<MacStyleCheckBox*>(widget);
        if (checkbox) {
            key   = checkbox->text();
            value = mConfig->value(key);
            checkbox->setChecked(value == "true" ? true : false);
            return;
        }
    } else if (typeName == "MacStyleComboBox") {
        MacStyleComboBox* comboBox = qobject_cast<MacStyleComboBox*>(widget);
        if (comboBox) {
            key   = comboBox->text();
            value = mConfig->value(key);
            comboBox->setCurrentText(value);
            return;
        }
    }
    qCritical() << "无法识别的类型:" << typeName;
}

void AudioHelperWidget::onThemeChanged()
{
    QColor hoverColor = ThemeManager::instance().primaryHoverColor();
    QColor selectColor = ThemeManager::instance().primaryColor();

    QString taskTabStyle = QString(
        "QTreeWidget {"
        "   background-color: transparent;"
        "   border: none;"
        "   outline: none;"
        "}" "QTreeWidget::item {"
        "   height: 32px;"
        "   color: " COLOR_TEXT_PRIMARY ";"
        "   background-color: transparent;"
        "}"
        "QTreeWidget::item:hover {"
        "   background-color: rgba(%1, %2, %3, 200);"
        "   margin: 1px 0;"
        "}"
        "QTreeWidget::item:selected {"
        "   background-color: rgba(%4, %5, %6, 200);"
        "   margin: 1px 0;"
        "   color: white;"
        "}"
        "QHeaderView {"
        "   background-color: transparent;"
        "   font-weight: %7;"
        "   font-size: %8px;"
        "}"
        "QTreeWidget::branch {"
        "   background: transparent;"
        "}")
        .arg(hoverColor.red()).arg(hoverColor.green()).arg(hoverColor.blue())
        .arg(selectColor.red()).arg(selectColor.green()).arg(selectColor.blue())
        .arg(QFont::Medium).arg(FONT_SIZE_XLARGE);

    mTaskTab->setStyleSheet(taskTabStyle);
}
