#include "SelectionDialog.h"
#include "AudioHelper.h"
#include "AudioDatabaseManager.h"

AudioHelper::AudioHelper(QWidget *parent)
    : ToolWidgetModel{parent}
    , mHomePage(new QWidget(this))
    , mPrefsPage(new QWidget(this))
    , mTaskTab(new QListWidget())
    , mRelatedList(new RelatedList())
{
    mServer = new AudioHelperServer(mRelatedList);

    resize(800, 480);
    setMinimumSize(800, 480);

    // 使用模版样式, 如果不使用直接添加Tab，程序会崩溃
    setDefaultStyle();

    addTab(mHomePage,  QIcon(":/ico/todo.svg"), "任务");
    addTab(mPrefsPage, QIcon(":/ico/user-settings.svg"), "偏好");

    initHomePage();
    initPrefsPage();

    connect(mTaskTab, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
    mTaskTab->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTaskTab->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    finalizeSetup();  // 检查并显示第一个页面

    mServer->start();
}

QString AudioHelper::queryConfig(const QString &key)
{
    return mConfig.value(key, QString());
}

void AudioHelper::initHomePage()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(mHomePage);
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QHBoxLayout *footLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(6, 0, 13, 0);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(mTaskTab);
    mainLayout->addLayout(footLayout);


    // 设置表头
    QLabel *keyLabel   = new QLabel("关联项");
    QLabel *typeLabel  = new QLabel("关联类型");
    QLabel *valueLabel = new QLabel("关联设备");

    typeLabel->setFixedWidth(TAG_DEFAULT_WIDTH);

    headerLayout->addWidget(keyLabel);
    headerLayout->addWidget(typeLabel);
    headerLayout->addWidget(valueLabel);

    keyLabel  ->setAlignment(Qt::AlignCenter);
    typeLabel ->setAlignment(Qt::AlignCenter);
    valueLabel->setAlignment(Qt::AlignCenter);

    keyLabel  ->setStyleSheet("font-size: 14px; font-weight: bold;");
    typeLabel ->setStyleSheet("font-size: 14px; font-weight: bold;");
    valueLabel->setStyleSheet("font-size: 14px; font-weight: bold;");


    // 从数据库读取数据
    RelatedList relatedList = AudioDatabaseManager::instance().queryItems("", "");
    qDebug() << "Loading related data：" << relatedList.length();

    // 将数据显示到ListWidget
    foreach (RelatedItem relatedItem, relatedList) {
        AudioTaskListWidgetItem *widget = new AudioTaskListWidgetItem(&relatedItem);
        QListWidgetItem *widgetItem = new QListWidgetItem(mTaskTab);
        mTaskTab->setItemWidget(widgetItem, widget);
        widgetItem->setSizeHint(widget->sizeHint());
        mRelatedList->append(relatedItem);
    }


    // 添加底部按钮
    footLayout->setContentsMargins(5, 5, 5, 0);

    MacStyleButton *addButton = new MacStyleButton("添加");
    MacStyleButton *delButton = new MacStyleButton("删除");
    MacStyleButton *chgButton = new MacStyleButton("修改");
    mGameButton = new MacStyleButton("标记游戏");
    mGameButton->setNormalColorBlue(true);

    footLayout->addWidget(new QLabel("关联项管理"));
    footLayout->addWidget(addButton);
    footLayout->addWidget(delButton);
    footLayout->addWidget(chgButton);
    footLayout->addStretch(1);
    footLayout->addWidget(mGameButton);
    // footLayout->addWidget(new QLabel("备份与恢复"));
    // footLayout->addWidget(new MacStyleButton("备份"));
    // footLayout->addWidget(new MacStyleButton("恢复"));

    connect(addButton,   SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(delButton,   SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(chgButton,   SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(mGameButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

}

void AudioHelper::initPrefsPage()
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
    loadConfigHandler(notifyCheckBox,           "true");
    loadConfigHandler(filterProcessCheckBox,    "true");
    loadConfigHandler(filterRepetitionCheckBox, "true");

    loadConfigHandler(modeComBox,  "窗口模式");
    loadConfigHandler(sceneComBox, "普通场景");


    // 连接槽 - 选择框
    connect(notifyCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(filterProcessCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    connect(filterRepetitionCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));

    // 连接槽 - 下拉框
    connect(modeComBox, SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxChanged(QString)));
    connect(sceneComBox,  SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxChanged(QString)));
}

void AudioHelper::onItemClicked(QListWidgetItem *item)
{
    // 获取 AudioTaskListWidgetItem
    AudioTaskListWidgetItem *widgetItem = qobject_cast<AudioTaskListWidgetItem*>(mTaskTab->itemWidget(item));

    if (widgetItem) {
        if (widgetItem->text(1) == "游戏")
            mGameButton->setText("取消标记");
        else
            mGameButton->setText("标记游戏");
    }
}

void AudioHelper::addRelatedItem()
{
    SelectionDialog selectionDialog(this);

    // 先选任务触发项
    if (selectionDialog.exec() != QDialog::Accepted) {
        qDebug() << "任务触发项：取消选择";
        return;
    }

    SelectionInfo *selectionInfo = selectionDialog.selectedOption();
    qDebug() << "任务触发项：" <<  selectionInfo->taskInfo.name << "|" << selectionInfo->taskInfo.path;

    // 再选任务关联项
    AudioChoiceDialog choiceDialog(this);
    if (choiceDialog.exec() != QDialog::Accepted) {
        qDebug() << "任务关联项：取消选择";
        return;
    }

    AudioDeviceInfo* deviceInfo = choiceDialog.selectedOption();
    qDebug() << "任务关联项：" <<  deviceInfo->name;

    // 结构化数据
    TypeInfo typeInfo{selectionInfo->type, ""};
    RelatedItem relatedItem{0, selectionInfo->taskInfo, typeInfo, *deviceInfo};

    // 添加到列表和数据库
    if (!AudioDatabaseManager::instance().insertItem(relatedItem))
    {
        return;
    }

    mRelatedList->append(relatedItem);

    // 添加到视图
    AudioTaskListWidgetItem *widget = new AudioTaskListWidgetItem(&relatedItem);
    QListWidgetItem *item = new QListWidgetItem(mTaskTab);
    mTaskTab->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
}

void AudioHelper::delRelatedItem()
{
    QListWidgetItem *item = mTaskTab->currentItem();

    if (!item)
        return;

    AudioTaskListWidgetItem *widgetItem = qobject_cast<AudioTaskListWidgetItem*>(mTaskTab->itemWidget(item));
    qDebug() << "删除:" << widgetItem->text(0) << "|" << widgetItem->text(2);
    if (!AudioDatabaseManager::instance().deleteItem(mRelatedList->at(mTaskTab->currentRow()).id))
    {
        qCritical() << "Failed to delete item:" <<AudioDatabaseManager::instance().lastError();
        return;
    }

    mRelatedList->removeAt(mTaskTab->currentRow());

    delete widgetItem;
    delete item;

    mTaskTab->clearSelection();
}

void AudioHelper::changeRelatedItem()
{
    QListWidgetItem *item = mTaskTab->currentItem();

    if (!item)
        return;

    AudioChoiceDialog choiceDialog(this);
    if (choiceDialog.exec() != QDialog::Accepted) {
        qDebug() << "更改任务关联项：取消选择";
        return;
    }

    AudioDeviceInfo* deviceInfo = choiceDialog.selectedOption();
    qDebug() << "更改任务关联项：" <<  deviceInfo->name;

    RelatedItem &relatedItem = (*mRelatedList)[mTaskTab->currentRow()];
    relatedItem.audioDeviceInfo = *deviceInfo;
    if (!AudioDatabaseManager::instance().updateItem(relatedItem))
    {
        qCritical() << "Failed to update item:" << AudioDatabaseManager::instance().lastError();
        return;
    }

    // 删除旧的
    mTaskTab->removeItemWidget(item);

    // 创建新的并关联
    AudioTaskListWidgetItem *widget = new AudioTaskListWidgetItem(&relatedItem);
    qDebug() << "更新:" << widget->text(0) << "|" << widget->text(2);
    mTaskTab->setItemWidget(item, widget);
    mTaskTab->clearSelection();
}

void AudioHelper::setGameTag(bool isGame)
{
    QListWidgetItem *item = mTaskTab->currentItem();

    if (!item)
        return;

    RelatedItem &relatedItem = (*mRelatedList)[mTaskTab->currentRow()];
    relatedItem.typeInfo.tag = isGame ? "游戏" : "";

    // 保存到数据库
    if (!AudioDatabaseManager::instance().updateItem(relatedItem))
    {
        qCritical() << "Failed to update item:" << AudioDatabaseManager::instance().lastError();
        return;
    }

    // 删除旧的
    mTaskTab->removeItemWidget(item);

    // 创建新的并关联
    AudioTaskListWidgetItem *widget = new AudioTaskListWidgetItem(&relatedItem);
    qDebug() << "标记:" << widget->text(0) << "|" << relatedItem.typeInfo.tag;
    mTaskTab->setItemWidget(item, widget);

    mGameButton->setText(isGame ? "取消标记" : "标记游戏");
}

void AudioHelper::buttonClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    qDebug() << "点击按钮：" << button->text();

    if (button->text() == "添加")
        addRelatedItem();
    else if (button->text() == "删除")
        delRelatedItem();
    else if (button->text() == "修改")
        changeRelatedItem();
    else if (button->text() == "标记游戏")
        setGameTag(true);
    else if (button->text() == "取消标记")
        setGameTag(false);
}

void AudioHelper::checkBoxChecked(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    QString key = checkBox->text();
    QString value = checked ? "true" : "false";
    qDebug() << (checked ? "勾选:" : "取消勾选:") << key;

    if (!AudioDatabaseManager::instance().saveConfig(key, value))
    {
        qWarning() << QString("save config %1(%2) to database failed").arg(key).arg(value);
    }
    mConfig[key] = value;
}

void AudioHelper::comboBoxChanged(QString currentText)
{
    MacStyleComboBox *comboBox = qobject_cast<MacStyleComboBox *>(sender());
    QString key = comboBox->text();
    qDebug() << key << "切换选项:" << comboBox->currentIndex() << currentText;

    if (!AudioDatabaseManager::instance().saveConfig(key, currentText))
    {
        qWarning() << QString("save config %1(%2) to database failed").arg(key).arg(currentText);
    }
    mConfig[key] = currentText;

    if (key == "任务模式")
    {
        if (currentText == "进程模式")
            mServer->setMode(AudioHelperServer::Process);
        else if (currentText == "窗口模式")
            mServer->setMode(AudioHelperServer::Windows);
        else
            mServer->setMode(AudioHelperServer::Smart);
    }

    if (key == "场景识别")
    {
        if (currentText == "游戏场景")
            mServer->setScene(AudioHelperServer::Entertainment);
        else if (currentText == "影音场景")
            mServer->setScene(AudioHelperServer::Audiovisual);
        else
            mServer->setScene(AudioHelperServer::Normal);
    }
}

template<typename T>
void AudioHelper::loadConfigHandler(T *widget, const QString &defaultValue)
{
    QString key, value;
    QString typeName = QString(typeid(widget).name()).split(' ')[1];

    if (typeName == "MacStyleCheckBox") {
        MacStyleCheckBox* checkbox = qobject_cast<MacStyleCheckBox*>(widget);
        if (checkbox) {
            key   = checkbox->text();
            value = AudioDatabaseManager::instance().queryConfig(key, defaultValue);
            mConfig.insert(key, value);
            checkbox->setChecked(value == "true" ? true : false);
            return;
        }
    } else if (typeName == "MacStyleComboBox") {
        MacStyleComboBox* comboBox = qobject_cast<MacStyleComboBox*>(widget);
        if (comboBox) {
            key   = comboBox->text();
            value = AudioDatabaseManager::instance().queryConfig(key, defaultValue);
            mConfig.insert(key, value);
            comboBox->setCurrentText(value);

            if (key == "任务模式")
            {
                if (value == "进程模式")
                    mServer->setMode(AudioHelperServer::Process);
                else if (value == "窗口模式")
                    mServer->setMode(AudioHelperServer::Windows);
                else
                    mServer->setMode(AudioHelperServer::Smart);
            }

            if (key == "场景识别")
            {
                if (value == "游戏场景")
                    mServer->setScene(AudioHelperServer::Entertainment);
                else if (value == "影音场景")
                    mServer->setScene(AudioHelperServer::Audiovisual);
                else
                    mServer->setScene(AudioHelperServer::Normal);
            }

            return;
        }
    }

    qCritical() << "无法识别的类型:" << typeName << defaultValue;
}
