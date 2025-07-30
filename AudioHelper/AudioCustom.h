#ifndef AUDIOCUSTOM_H
#define AUDIOCUSTOM_H

/**
 * @file AudioCustom.h
 * @author Asteri5m
 * @date 2025-02-08 0:20:14
 * @brief AudioHelper的一些自定义小组件
 */

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QPainter>

#define TAG_DEFAULT_WIDTH 120

class TagLabel : public QWidget {
    Q_OBJECT
public:
    enum Theme {
        Pink,
        Blue,
        Green,
        Yellow,
        Purple,
        Orange,
        Default
    };

    explicit TagLabel(const QString &text=QString(),Theme theme=Blue, QWidget *parent = nullptr)
        : QWidget(parent), mLabel(new QLabel(text))
    {

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setAlignment(Qt::AlignCenter);

        mLabel->setFixedSize(60, 20);
        mLabel->setStyleSheet(generateStyleSheet(theme));
        mLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(mLabel);
    }

    QString generateStyleSheet(Theme theme)
    {
        QString back_color;
        QString text_color;

        switch (theme) {
        case Pink:
            back_color = "#FFDDFF";
            text_color = "#FF00FF";
            break;
        case Blue:
            back_color = "#CEFDFF";
            text_color = "#128DE2";
            break;
        case Green:
            back_color = "#E3F9E9";
            text_color = "#2BA471";
            break;
        case Yellow:
            back_color = "#FFFAD5";
            text_color = "#746C3D";
            break;
        case Purple:
            back_color = "#EFE3FC";
            text_color = "#9933FF";
            break;
        case Orange:
            back_color = "#FFE5E0";
            text_color = "#FF6347";
            break;
        default:
            back_color = "#FFFBCE";
            text_color = "#FDAF32";
        }

        return QString(
                   "background-color: %1;"
                   "border-radius: 5px;"
                   "border: 1px solid %2;"
                   "color: %2;"
                   ).arg(back_color).arg(text_color);
    }

    void setTheme(Theme theme)
    {
        mLabel->setStyleSheet(generateStyleSheet(theme));
    }

    QString text() const {
        QString text = mLabel->text();
        text.remove(" ");
        return text;
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        painter.setBrush(Qt::transparent);
        painter.drawRect(this->rect());
    }

private:
    QLabel *mLabel;

};

inline QMap<QString, TagLabel::Theme> TagTheme =  {
    {"进程", TagLabel::Blue},
    {"窗口", TagLabel::Green},
    {"文件夹", TagLabel::Yellow},
    {"文件", TagLabel::Purple},
    {"游戏", TagLabel::Pink},
    {"影音", TagLabel::Orange},
    };


struct TaskInfo {
    QString name;
    QString path;
    qint64 survivalTime;
};

struct AudioDeviceInfo {
    QString name;
    QString id;
};

struct TypeInfo
{
    QString type;
    QString tag;
};

struct RelatedItem {
    uint id;            // 数据库id
    TaskInfo taskInfo;
    TypeInfo typeInfo;
    AudioDeviceInfo audioDeviceInfo;
};

typedef QList<RelatedItem> RelatedList;

#include <QStyledItemDelegate>
#include <QPainter>

// 自定义委托，用于在每个路径项后绘制分隔符
class PathDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    PathDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyledItemDelegate::paint(painter, option, index);

        // 获取当前项的矩形区域
        QRect rect = option.rect;

        // 如果不是最后一个项，绘制分隔符 '/'
        if (index.row() < index.model()->rowCount() - 1) {
            painter->save();
            painter->setPen(Qt::gray);

            // 在右侧绘制 '/'
            QRect slashRect(rect.right() - 10, rect.top(), 10, rect.height());
            painter->drawText(slashRect, Qt::AlignCenter, "/");
            painter->restore();
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        // 调整大小以适应分隔符
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setWidth(size.width() + 10); // 为分隔符预留空间
        return size;
    }
};

#include <QDir>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QFileIconProvider>

class DiskWidget : public QWidget {
    Q_OBJECT

public:
    DiskWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(3);

        // 创建导航栏
        mPathLineView = new QListView(this);
        mPathLineEdit = new QLineEdit(this);

        mPathLineView->setFlow(QListView::LeftToRight);
        mPathLineView->setWrapping(false);
        mPathLineView->setSelectionMode(QListView::SingleSelection);
        mPathLineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        layout->addWidget(mPathLineView);
        layout->addWidget(mPathLineEdit);

        mPathLineView->setFixedHeight(30);
        mPathLineEdit->setFixedHeight(30);
        mPathLineEdit->hide();

        // 设置自定义委托
        mPathLineView->setItemDelegate(new PathDelegate(this));

        // 路径模型
        mPathLineModel = new QStringListModel(this);
        mPathLineView->setModel(mPathLineModel);

        // 文件视图：QListView 显示当前文件夹内容
        mPathListView = new QListView(this);
        layout->addWidget(mPathListView);

        // 文件系统模型
        mPathListModel = new QStandardItemModel(this);
        mPathListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mPathListView->setModel(mPathListModel);

        // 初始显示根目录 QDir::rootPath()
        setDirectory("");

        // 连接信号槽：点击路径列表项时切换到对应目录
        connect(mPathLineView, &QListView::clicked, this, &DiskWidget::onPathItemClicked);

        // 双击进入文件夹，单击选中
        connect(mPathListView, &QListView::doubleClicked, this, &DiskWidget::onItemDoubleClicked);
        connect(mPathListView, &QListView::clicked, this, &DiskWidget::onItemClicked);

        // 双击路径列表的空白处以手动输入路径
        mPathLineView->viewport()->installEventFilter(this);
        // 连接信号槽: 当路径编辑框按回车键时，跳转到输入的路径
        connect(mPathLineEdit, &QLineEdit::returnPressed, this, &DiskWidget::onPathEntered);
        connect(mPathLineEdit, &QLineEdit::editingFinished, this, &DiskWidget::onEditingFinished);

    }

protected:
    // 捕捉双击事件，在路径栏双击空白处时，弹出输入框
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (obj == mPathLineView->viewport() && event->type() == QEvent::MouseButtonDblClick) {
            mPathLineEdit->setText(mCurrentPath);
            mPathLineView->hide();
            mPathLineEdit->show();
            mPathLineEdit->setFocus();
        }
        return QWidget::eventFilter(obj, event);
    }

private slots:
    // 用户点击路径中的某一部分时，切换到该路径
    void onPathItemClicked(const QModelIndex &index) {
        QStringList pathList = mCurrentPath.split("/", Qt::SkipEmptyParts);
        QString newPath = pathList.mid(0, index.row()).join("/");
        setDirectory(newPath);
    }

    // 用户双击文件夹时进入该目录
    void onItemDoubleClicked(const QModelIndex &index) {
        QString newPath = QDir(mCurrentPath).filePath(index.data().toString());
        QFileInfo fileInfo(newPath);
        if (fileInfo.isDir()) {
            setDirectory(newPath);
        }
    }

    // 输入完后自动切会列表
    void onEditingFinished() {
        mPathLineEdit->hide();
        mPathLineView->show();
    }

    // 用户按回车输入路径时的处理函数
    void onPathEntered() {
        QString enteredPath = mPathLineEdit->text();
        mPathLineEdit->hide();
        mPathLineView->show();
        setDirectory(QDir::cleanPath(enteredPath));
    }

    void onItemClicked(const QModelIndex &index) {
        QString currentText = QDir(mCurrentPath).filePath(index.data().toString());

        QFileInfo fileInfo(currentText);

        if (fileInfo.isDir())
        {
            TaskInfo taskInfo{currentText, currentText};
            emit currentChanged(taskInfo, "文件夹");
        }
        else
        {
            TaskInfo taskInfo{fileInfo.baseName(), currentText};
            emit currentChanged(taskInfo, "文件");
        }
    }

signals:
    void currentChanged(const TaskInfo &taskInfo, const QString &type);

private:
    // 更新当前显示的路径
    void setDirectory(const QString &path) {
        QDir dir(path);
        if (path.endsWith(":"))
            dir = QDir(path + "/");

        // 相对路径会导致图标解析失败
        dir = QDir(dir.canonicalPath());

        if (!dir.exists()) {
            QMessageBox::warning(this, tr("路径错误"), tr("该路径不存在！"));
            return;
        }

        mCurrentPath = path=="" ? "" : dir.absolutePath();

        // 更新路径模型
        QStringList pathList = mCurrentPath.split("/", Qt::SkipEmptyParts);
        pathList.insert(0, "此设备");
        mPathLineModel->setStringList(pathList);

        // 始终滚动到路径的最后一个部分
        mPathLineView->scrollTo(mPathLineModel->index(mPathLineModel->rowCount() - 1), QAbstractItemView::PositionAtCenter);

        // 更新文件视图
        QFileIconProvider iconProvider;
        dir.setSorting(QDir::DirsFirst);

        // 处理根目录
        QFileInfoList fileInfoList;
        if (path == "")
            fileInfoList = QDir::drives();
        else
            fileInfoList =  dir.entryInfoList(QDir::AllEntries | QDir::NoDot);

        // 列出文件和文件夹
        mPathListModel->clear();
        foreach (const QFileInfo fileInfo, fileInfoList) {
            QStandardItem *item;
            if (path == "")
                // drives的fileName是空的
                item = new QStandardItem(iconProvider.icon(fileInfo), fileInfo.absoluteFilePath());
            else
                item = new QStandardItem(iconProvider.icon(fileInfo), fileInfo.fileName());
            mPathListModel->appendRow(item);
        }

        mPathListView->update();
    }

    QString mCurrentPath;              // 当前路径
    QLineEdit *mPathLineEdit;          // 路径列表编辑栏
    QListView *mPathLineView;          // 路径列表视图
    QListView *mPathListView;          // 文件列表视图
    QStringListModel *mPathLineModel;  // 路径列表模型
    QStandardItemModel *mPathListModel;// 文件列表模型
};


#include "AudioManager.h"
#include "CustomWidget.h"
class AudioChoiceDialog : public QDialog {
    Q_OBJECT

public:
    explicit AudioChoiceDialog(const QString &text=QString(), QWidget *parent = nullptr) : QDialog(parent) {
        // 设置对话框标题
        setWindowTitle("请选择音频设备");

        // 创建一个 QLabel 来描述下拉框的用途
        QLabel* label;
        if (text.isEmpty())
            label = new QLabel("请选择需要关联的音频设备:", this);
        else
            label = new QLabel(text, this);

        // 创建 QComboBox 并添加选项
        mComboBox = new MacStyleComboBox(this);
        mDeviceList = new AudioDeviceList();
        AudioManager::getAudioOutDeviceList(mDeviceList);
        for (auto it = mDeviceList->constBegin(); it != mDeviceList->constEnd(); ++it) {
            mComboBox->addItem(it.key());
        }

        // 创建按钮框，包含 "确定" 和 "取消" 按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        MacStyleButton *checkButton  = new MacStyleButton("确定");
        MacStyleButton *cancelButton = new MacStyleButton("取消");

        checkButton->setNormalColorBlue(true);

        buttonLayout->addWidget(checkButton);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(cancelButton);


        // 连接 "确定" 和 "取消" 按钮的信号到相应的槽
        connect(checkButton,  SIGNAL(clicked()), this, SLOT(accept()));  // 点击 "确定"
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));  // 点击 "取消"

        // 创建布局并将部件添加到布局中
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(label);
        layout->addWidget(mComboBox);
        layout->addLayout(buttonLayout);

        setFixedSize(sizeHint());
    }

    // 返回 QComboBox 的当前选择
    AudioDeviceInfo* selectedOption() const {
        QString name = mComboBox->currentText();
        QString id = (*mDeviceList).value(name);

        return new AudioDeviceInfo{name, id};
    }

private:
    QComboBox* mComboBox;
    AudioDeviceList *mDeviceList;
};

class TagSwitchDialog : public QDialog {
    Q_OBJECT

public:
    explicit TagSwitchDialog(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置对话框标题
        setWindowTitle("请选择应用场景");

        // 创建一个 QLabel 来描述下拉框的用途
        QLabel* label = new QLabel("请选择该任务的应用场景:", this);

        // 创建 QComboBox 并添加选项
        mComboBox = new MacStyleComboBox(this);
        mComboBox->addItem("游戏");
        mComboBox->addItem("影音");

        // 创建按钮框，包含 "确定" 和 "取消" 按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        MacStyleButton *checkButton  = new MacStyleButton("确定");
        MacStyleButton *cancelButton = new MacStyleButton("取消");

        checkButton->setNormalColorBlue(true);

        buttonLayout->addWidget(checkButton);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(cancelButton);


        // 连接 "确定" 和 "取消" 按钮的信号到相应的槽
        connect(checkButton,  SIGNAL(clicked()), this, SLOT(accept()));  // 点击 "确定"
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));  // 点击 "取消"

        // 创建布局并将部件添加到布局中
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(label);
        layout->addWidget(mComboBox);
        layout->addLayout(buttonLayout);

        setFixedSize(sizeHint());
    }

    // 返回 QComboBox 的当前选择
    QString selectedOption() const {
        return mComboBox->currentText();
    }

private:
    QComboBox* mComboBox;
};

#endif // AUDIOCUSTOM_H
