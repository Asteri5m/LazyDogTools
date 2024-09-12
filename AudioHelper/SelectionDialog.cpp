#include "SelectionDialog.h"


SelectionDialog::SelectionDialog(QWidget *parent)
    : QDialog{parent}
    , mTaskMonitor(new TaskMonitor(this))
{
    setWindowTitle("添加关联项");
    setFixedSize(460, 520);
    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *footLayout = new QHBoxLayout();

    QTabWidget *tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);
    layout->addLayout(footLayout);

    // 创建第一个选项卡“进程”
    QWidget *processTab = new QWidget();
    QVBoxLayout *processLayout = new QVBoxLayout(processTab);
    processLayout->setContentsMargins(6, 6, 5, 5);
    QListView *processListView = new QListView();
    processListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    processListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    processListView->setModel(mTaskMonitor->getProcessModel());
    processLayout->addWidget(processListView);

    // 创建第二个选项卡“窗口”
    QWidget *windowTab = new QWidget();
    QVBoxLayout *windowLayout = new QVBoxLayout(windowTab);
    windowLayout->setContentsMargins(6, 6, 5, 5);
    QListView *windowListView = new QListView();
    windowListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    windowListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    windowListView->setModel(mTaskMonitor->getWindowsModel());
    windowLayout->addWidget(windowListView);

    // 创建第三个选项卡“磁盘”
    QWidget *diskTab = new QWidget();
    QVBoxLayout *diskLayout = new QVBoxLayout(diskTab);
    diskLayout->setContentsMargins(6, 6, 5, 5);
    DiskWidget *diskWidget = new DiskWidget();
    diskLayout->addWidget(diskWidget);

    // 将选项卡添加到 QTabWidget
    tabWidget->addTab(processTab, "进程");
    tabWidget->addTab(windowTab, "窗口");
    tabWidget->addTab(diskTab, "磁盘");

    MacStyleButton *checkButton  = new MacStyleButton("添加");
    MacStyleButton *cancelButton = new MacStyleButton("取消");
    MacStyleButton *renewButton  = new MacStyleButton("刷新");

    checkButton->setNormalColorBlue(true);
    renewButton->setNormalColorBlue(true);

    //添加底部按钮
    footLayout->addWidget(checkButton);
    footLayout->addWidget(renewButton);
    footLayout->addStretch(1);
    footLayout->addWidget(cancelButton);

    // 连接槽
    connect(diskWidget, SIGNAL(currentChanged(QString)), this, SLOT(updateSelection(QString)));
    connect(checkButton,  SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(renewButton, SIGNAL(clicked()), mTaskMonitor, SLOT(update()));
    connect(processListView, SIGNAL(clicked(QModelIndex)), this, SLOT(onProcessItemClicked(QModelIndex)));
    connect(windowListView,  SIGNAL(clicked(QModelIndex)), this, SLOT(onWindowItemClicked(QModelIndex)));

    // 设置过滤器并刷新
    QStringList filter;
    filter << "C:/Windows/" << "C:/Program Files/WindowsApps/";
    mTaskMonitor->setFilter(filter, TaskMonitor::Process);
    mTaskMonitor->setFilter(filter, TaskMonitor::Windows);
    mTaskMonitor->update();
}

QString SelectionDialog::selectedOption() const
{
    return mSelectedOption;
}

void SelectionDialog::updateSelection(const QString &text)
{
    mSelectedOption = text;
}

void SelectionDialog::onWindowItemClicked(const QModelIndex &index)
{
    updateSelection(mTaskMonitor->filePath(index, TaskMonitor::Windows));
}

void SelectionDialog::onProcessItemClicked(const QModelIndex &index)
{
    updateSelection(mTaskMonitor->filePath(index, TaskMonitor::Process));
}

