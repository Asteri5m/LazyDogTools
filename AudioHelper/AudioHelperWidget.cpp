/**
 * @file AudioHelperWidget.cpp
 * @author Asteri5m
 * @date 2025-02-08 0:07:54
 * @brief AudioHelper的交互窗口
 */

#include "AudioHelperWidget.h"
#include "ToolManager.h"
#include "SelectionDialog.h"

AudioHelperWidget::AudioHelperWidget(RelatedList *relatedList, QMap<QString, QString> *config, AudioDatabase* database, QWidget *parent)
    : ToolWidgetModel{parent}
    , mHomePage(new QWidget(this))
    , mPrefsPage(new QWidget(this))
    , mTaskTab(new TableWidget())
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

    addTab(mHomePage,  QIcon(":/ico/todo.svg"), "任务");
    addTab(mPrefsPage, QIcon(":/ico/user-settings.svg"), "偏好");

    initHomePage();
    initPrefsPage();

    connect(mTaskTab, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onItemClicked(QTableWidgetItem*)));

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
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QHBoxLayout *footLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(6, 0, 13, 0);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(mTaskTab);
    mainLayout->addLayout(footLayout);

    // 设置委托
    TableRowDelegate *delegate = new TableRowDelegate(mTaskTab);
    mTaskTab->setItemDelegate(delegate);

    // 设置表头
    mTaskTab->setColumnCount(3);
    mTaskTab->setHorizontalHeaderLabels({"关联项", "关联类型", "关联设备"});
    mTaskTab->setRowCount(mRelatedList->length());
    mTaskTab->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    mTaskTab->setColumnWidth(1, TAG_DEFAULT_WIDTH);

    for (int i = 0; i < mRelatedList->length(); i++) {
        const RelatedItem *relatedItem = &mRelatedList->at(i);
        // 第一列：图标+名称
        QFileIconProvider iconProvider;
        QIcon icon = iconProvider.icon(QFileInfo(relatedItem->taskInfo.path));
        QTableWidgetItem *item = new QTableWidgetItem(relatedItem->taskInfo.name);
        item->setIcon(icon);  // 设置图标
        mTaskTab->setItem(i, 0, item);
        // 第二列：标签
        mTaskTab->setItem(i, 1, new QTableWidgetItem());
        QString tag = relatedItem->typeInfo.tag  == "" ? relatedItem->typeInfo.type : relatedItem->typeInfo.tag;
        TagLabel *tagLabel = new TagLabel(tag.length() > 2 ? tag : QString(tag).insert(1, "   "));
        tagLabel->setFixedWidth(TAG_DEFAULT_WIDTH);
        tagLabel->setTheme(TagTheme.value(tag, TagLabel::Theme::Default));
        mTaskTab->setIndexWidget(mTaskTab->model()->index(i, 1), tagLabel);
        // 第三列：设备名
        mTaskTab->setItem(i, 2, new QTableWidgetItem(relatedItem->audioDeviceInfo.name));
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
    connect(mTagButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

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

    layout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(20, 10, 10, 10);

    // 模式
    NoBorderGroupBox *modeGroupBox = new NoBorderGroupBox("模式");
    QGridLayout *modeLayout = new QGridLayout(modeGroupBox);
    modeLayout->addWidget(new QLabel("选择触发任务的算法模式："), 0, 0);

    // 模式选项
    MacStyleComboBox *modeComBox = new MacStyleComboBox("任务模式");
    modeComBox->addItem("进程模式");
    modeComBox->addItem("窗口模式");
    modeComBox->addItem("智能模式");
    modeLayout->addWidget(modeComBox, 0, 1);
    modeLayout->setColumnStretch(2, 1);


    // 场景识别
    NoBorderGroupBox *sceneGroupBox = new NoBorderGroupBox("场景");
    QGridLayout *sceneLayout = new QGridLayout(sceneGroupBox);
    sceneLayout->addWidget(new QLabel("多条关联规则命中时的场景选择："), 0, 0);

    // 场景选项
    MacStyleComboBox *sceneComBox = new MacStyleComboBox("场景识别");
    sceneComBox->addItem("游戏场景");
    sceneComBox->addItem("影音场景");
    sceneComBox->addItem("普通场景");
    sceneLayout->addWidget(sceneComBox, 0, 1);
    sceneLayout->setColumnStretch(2, 1);


    // 通知
    NoBorderGroupBox *notifyGroupBox = new NoBorderGroupBox("通知");
    QGridLayout *notifyLayout = new QGridLayout(notifyGroupBox);
    MacStyleCheckBox *notifyCheckBox = new MacStyleCheckBox("切换时通知");
    notifyLayout->addWidget(notifyCheckBox, 0, 0);


    // 过滤
    NoBorderGroupBox *filterGroupBox = new NoBorderGroupBox("过滤");
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

void AudioHelperWidget::onItemClicked(QTableWidgetItem *item)
{
    // 获取被点击的行
    int row = item->row();

    // 获取该行第二列的 widget
    QWidget *widget = mTaskTab->cellWidget(row, 1);  // 第二列的索引为1

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
        if (showMessage(this, "重复添加", "该选项已经在任务列表中，无法重复添加！\n\n是否继续添加其它项?\n", MessageType::Warning, "继续", "返回") == QMessageBox::Accepted)
            return addRelatedItem();
        return;
    }

    // 再选任务关联项
    AudioChoiceDialog choiceDialog(this);
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

    // 获取当前表格的行数
    int rowCount = mTaskTab->rowCount();
    // 插入新行
    mTaskTab->insertRow(rowCount);  // 在最后插入一行
    // 第一列：图标+名称
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(QFileInfo(relatedItem.taskInfo.path));
    QTableWidgetItem *item = new QTableWidgetItem(relatedItem.taskInfo.name);
    item->setIcon(icon);  // 设置图标
    mTaskTab->setItem(rowCount, 0, item);
    // 第二列：标签
    mTaskTab->setItem(rowCount, 1, new QTableWidgetItem());
    QString tag = relatedItem.typeInfo.tag  == "" ? relatedItem.typeInfo.type : relatedItem.typeInfo.tag;
    TagLabel *tagLabel = new TagLabel(tag.length() > 2 ? tag : QString(tag).insert(1, "   "));
    tagLabel->setFixedWidth(TAG_DEFAULT_WIDTH);
    tagLabel->setTheme(TagTheme.value(tag, TagLabel::Theme::Default));
    mTaskTab->setIndexWidget(mTaskTab->model()->index(rowCount, 1), tagLabel);
    // 第三列：设备名
    mTaskTab->setItem(rowCount, 2, new QTableWidgetItem(relatedItem.audioDeviceInfo.name));
}

void AudioHelperWidget::delRelatedItem()
{
    QTableWidgetItem *item = mTaskTab->currentItem();

    if (!item)
        return;

    const RelatedItem *relatedItem = &mRelatedList->at(mTaskTab->currentRow());
    qInfo() << "delete item:" << relatedItem->taskInfo.name << "-" << relatedItem->audioDeviceInfo.name;
    if (!mDatabase->deleteItem(relatedItem->id))
    {
        qCritical() << "Failed to delete item:" << mDatabase->lastError();
        return;
    }

    mRelatedList->removeAt(mTaskTab->currentRow());
    mTaskTab->removeRow(mTaskTab->currentRow());
    mTaskTab->clearSelection();
    mTaskTab->setCurrentItem(nullptr);
}

void AudioHelperWidget::changeRelatedItem()
{
    QTableWidgetItem *item = mTaskTab->currentItem();

    if (!item)
        return;

    AudioChoiceDialog choiceDialog(this);
    if (choiceDialog.exec() != QDialog::Accepted) {
        qInfo() << "任务" << item->text() << "更改关联项: 取消选择";
        return;
    }

    AudioDeviceInfo* deviceInfo = choiceDialog.selectedOption();
    qInfo() << "任务"  << item->text() << "更改关联项: " <<  deviceInfo->name;

    RelatedItem &relatedItem = (*mRelatedList)[mTaskTab->currentRow()];
    relatedItem.audioDeviceInfo = *deviceInfo;
    if (!mDatabase->updateItem(relatedItem))
    {
        qCritical() << "Failed to update item:" << mDatabase->lastError();
        return;
    }

    // 修改UI为新的值
    mTaskTab->setItem(item->row(), 2, new QTableWidgetItem(relatedItem.audioDeviceInfo.name));
    mTaskTab->clearSelection();
    mTaskTab->setCurrentItem(nullptr);
}

void AudioHelperWidget::setSceneTag(bool isAdd)
{
    QTableWidgetItem *item = mTaskTab->currentItem();

    if (!item)
        return;

    int current_row = mTaskTab->currentRow();
    RelatedItem &relatedItem = (*mRelatedList)[current_row];
    if (isAdd)
    {
        TagSwitchDialog tagSwitchDialog(this);

        if (tagSwitchDialog.exec() != QDialog::Accepted) {
            qDebug() << "场景关联: 取消选择";
            return;
        }

        relatedItem.typeInfo.tag = tagSwitchDialog.selectedOption();
    }
    else
        relatedItem.typeInfo.tag = "";


    // 保存到数据库
    if (!mDatabase->updateItem(relatedItem))
    {
        qCritical() << "Failed to update item:" << mDatabase->lastError();
        return;
    }

    // 删除旧的
    mTaskTab->removeCellWidget(current_row, 1);

    // 创建新的并关联
    QString tag = relatedItem.typeInfo.tag  == "" ? relatedItem.typeInfo.type : relatedItem.typeInfo.tag;
    TagLabel *tagLabel = new TagLabel(tag.length() > 2 ? tag : QString(tag).insert(1, "   "));
    tagLabel->setFixedWidth(TAG_DEFAULT_WIDTH);
    tagLabel->setTheme(TagTheme.value(tag, TagLabel::Theme::Default));
    mTaskTab->setIndexWidget(mTaskTab->model()->index(current_row, 1), tagLabel);
    qDebug() << "场景关联:" << relatedItem.taskInfo.name << "|" << relatedItem.typeInfo.tag;
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
