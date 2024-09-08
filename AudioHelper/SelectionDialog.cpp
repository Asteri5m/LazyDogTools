#include "SelectionDialog.h"
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QFileDialog>

SelectionDialog::SelectionDialog(QWidget *parent)
    : QDialog{parent}
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
    QLabel *processLabel = new QLabel("这是进程选项卡的内容");
    processLayout->addWidget(processLabel);

    // 创建第二个选项卡“窗口”
    QWidget *windowTab = new QWidget();
    QVBoxLayout *windowLayout = new QVBoxLayout(windowTab);
    QLabel *windowLabel = new QLabel("这是窗口选项卡的内容");
    windowLayout->addWidget(windowLabel);

    // 将两个选项卡添加到 QTabWidget
    tabWidget->addTab(processTab, "进程");
    tabWidget->addTab(windowTab, "窗口");


    MacStyleButton *checkButton  = new MacStyleButton("添加");
    MacStyleButton *cancelButton = new MacStyleButton("取消");
    MacStyleButton *renewButton  = new MacStyleButton("刷新");
    MacStyleButton *fileButton   = new MacStyleButton("从文件添加");

    checkButton->setNormalColorBlue(true);
    fileButton ->setNormalColorBlue(true);
    renewButton->setNormalColorBlue(true);

    footLayout->addWidget(checkButton);
    footLayout->addWidget(fileButton);
    footLayout->addWidget(renewButton);
    footLayout->addStretch();
    footLayout->addWidget(cancelButton);

    // 连接按钮信号到槽
    connect(checkButton,  SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    // 连接按钮点击事件
    QObject::connect(fileButton, &MacStyleButton::clicked, [&]() {
        // 创建文件对话框，可以选择文件或目录
        QFileDialog dialog(this);

        // 设置为可以选择文件和目录
        // dialog.setFileMode(QFileDialog::AnyFile);
        // dialog.setFileMode(QFileDialog::Directory);
        // dialog.setOption(QFileDialog::ShowDirsOnly, false);

        // 设置文件过滤器（可以根据需要修改）
        dialog.setNameFilters(QStringList() << "可执行文件 (*.exe *.bat *.lnk)"
                                            << "所有文件 (*.*)");

        // 设置默认过滤器
        dialog.setDefaultSuffix("exe");

        // 允许用户选择目录或者文件
        dialog.setWindowTitle("选择文件或目录");

        // 打开对话框并获取用户选择的路径
        if (dialog.exec() == QDialog::Accepted) {
            // 获取选中的路径
            QStringList selectedFiles = dialog.selectedFiles();

            // 这里只显示选择的第一个文件或目录
            QString selectedPath = selectedFiles.isEmpty() ? "" : selectedFiles.first();

            // 弹出消息框显示选择的路径
            qDebug() << "选择路径: " << selectedPath;
        } else {
            qDebug() << "没有选择任何文件或目录";
        }
    });

    // 当用户选择时更新选择结果
    // connect(comboBox, &QComboBox::currentTextChanged, this, &SelectionDialog::updateSelection);

}

QString SelectionDialog::getSelectedOption() const
{
    return mSelectedOption;
}

void SelectionDialog::updateSelection(const QString &text)
{
    mSelectedOption = text;
}


