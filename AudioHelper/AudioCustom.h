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

    explicit TagLabel(const QString &text = QString(), Theme theme = Blue, QWidget *parent = nullptr)
        : QWidget(parent), mText(text), mTheme(theme)
    {
        // 设置最小尺寸为62x22
        setMinimumSize(62, 22);
        // 设置固定尺寸策略，但允许在需要时扩展
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void setTheme(Theme theme)
    {
        mTheme = theme;
        update(); // 触发重绘
    }

    QString text() const {
        QString text = mText;
        text.remove(" ");
        return text;
    }

    void setText(const QString &text) {
        mText = text;
        update(); // 触发重绘
    }

    QSize sizeHint() const override
    {
        return QSize(62, 22); // 返回建议尺寸
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

        // 计算中心区域 (60x20)
        QRect tagRect = rect();
        tagRect.setWidth(60);
        tagRect.setHeight(20);
        tagRect.moveCenter(rect().center());

        // 绘制背景
        drawBackground(painter, tagRect);

        // 绘制文本
        drawText(painter, tagRect);
    }

private:
    void drawBackground(QPainter &painter, const QRect &rect)
    {
        QColor bgColor, borderColor;

        // 根据主题设置颜色
        switch (mTheme) {
        case Pink:
            bgColor = QColor("#FFDDFF");
            borderColor = QColor("#FF00FF");
            break;
        case Blue:
            bgColor = QColor("#CEFDFF");
            borderColor = QColor("#128DE2");
            break;
        case Green:
            bgColor = QColor("#E3F9E9");
            borderColor = QColor("#2BA471");
            break;
        case Yellow:
            bgColor = QColor("#FFFAD5");
            borderColor = QColor("#746C3D");
            break;
        case Purple:
            bgColor = QColor("#EFE3FC");
            borderColor = QColor("#9933FF");
            break;
        case Orange:
            bgColor = QColor("#FFE5E0");
            borderColor = QColor("#FF6347");
            break;
        default:
            bgColor = QColor("#FFFBCE");
            borderColor = QColor("#FDAF32");
        }

        // 绘制圆角矩形背景
        painter.setPen(QPen(borderColor, 1)); // 1px边框
        painter.setBrush(bgColor);
        painter.drawRoundedRect(rect, 5, 5); // 5px圆角
    }

    void drawText(QPainter &painter, const QRect &rect)
    {
        // 设置文本颜色（与边框相同）
        QColor textColor = borderColorForTheme(mTheme);
        painter.setPen(textColor);

        // 设置字体
        QFont font = painter.font();
        font.setPointSizeF(9); // 固定字体大小
        painter.setFont(font);

        // 绘制居中文本
        painter.drawText(rect, Qt::AlignCenter, mText);
    }

    QColor borderColorForTheme(Theme theme) const
    {
        switch (theme) {
        case Pink: return QColor("#FF00FF");
        case Blue: return QColor("#128DE2");
        case Green: return QColor("#2BA471");
        case Yellow: return QColor("#746C3D");
        case Purple: return QColor("#9933FF");
        case Orange: return QColor("#FF6347");
        default: return QColor("#FDAF32");
        }
    }

private:
    QString mText;
    Theme mTheme;
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
    MacStyleComboBox* mComboBox;
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
    MacStyleComboBox* mComboBox;
};

#endif // AUDIOCUSTOM_H
