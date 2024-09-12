#ifndef AUDIOCUSTOM_H
#define AUDIOCUSTOM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QResizeEvent>

#define TAG_DEFAULT_WIDTH 120

class TagLabel : public QWidget
{
public:
    enum Theme {
        Pink,
        Blue,
        Green,
        Yellow,
        Purple,
        Default
    };

    explicit TagLabel(const QString &text=QString(),Theme theme=Blue, QWidget *parent = nullptr)
        : QWidget(parent), mLabel(new QLabel(text))
    {

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setAlignment(Qt::AlignCenter);

        mLabel->setFixedSize(60, 20);
        mLabel->setStyleSheet(getStyleSheet(theme));
        mLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(mLabel);
    }

    QString getStyleSheet(Theme theme)
    {
        QString back_color = "#fffbce";
        QString text_color = "#fdaf32";

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
        mLabel->setStyleSheet(getStyleSheet(theme));
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
};


#include <QFileIconProvider>
#include <QPixmap>
class AudioTaskListWidgetItem : public QWidget {
    Q_OBJECT
public:
    AudioTaskListWidgetItem(const QString& col1, const QString& col2, const QString& col3, QWidget *parent = nullptr)
        : QWidget(parent), originalText1(col1), originalText2(col2), originalText3(col3) {

        // 创建水平布局
        layout = new QHBoxLayout(this);
        layout->setContentsMargins(5, 0, 0, 0);

        // 创建每列的 QLabel
        label1 = new QLabel(col1);
        label2 = new TagLabel(col2.length() > 2 ? col2 : QString(col2).insert(1, "   "));
        label3 = new QLabel(col3);

        // 获取文件的图标
        QFileIconProvider fileIconProvider;
        QIcon icon = fileIconProvider.icon(QFileInfo(col1));
        QList<QSize> sizes = icon.availableSizes();
        QPixmap pixmap = icon.pixmap(sizes.first());
        QLabel *iconLabel = new QLabel();
        iconLabel->setPixmap(pixmap);

        // 第一列由 图标+描述 组成
        QHBoxLayout *headerLayout = new QHBoxLayout();
        headerLayout->setAlignment(Qt::AlignLeft);
        headerLayout->addWidget(iconLabel);
        headerLayout->addWidget(label1);


        // 添加到布局
        layout->addLayout(headerLayout);
        layout->addWidget(label2);
        layout->addWidget(label3);

        label2->setStyleSheet(
            "border-left: 1px solid #ececec;"
            "border-right: 1px solid #ececec;"
            );
        label2->setFixedWidth(TAG_DEFAULT_WIDTH);
        label2->setTheme(TagTheme.value(col2, TagLabel::Theme::Default));

        setLayout(layout);
    }

    QSize sizeHint() const override {
        auto width = this->parentWidget()->width();
        return QSize(width, 32);  // 自定义宽度和高度
    }

    QString text(int col) const {
        switch (col) {
        case 0:
            return originalText1;
        case 1:
            return originalText2;
        case 2:
            return originalText3;
        default:
            qWarning() << QString("Parameter error, no matching case <col (%1)>").arg(1).toLocal8Bit().constData();
            return "";
        };
    }

protected:
    // 重写 resizeEvent 以动态调整列的宽度和省略号显示
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);

        int totalWidth = event->size().width();  // 当前控件总宽度
        int columnWidth = (totalWidth - label2->width()) / 2;

        // 动态调整每个 QLabel 的文本长度并添加省略号 需要额外留出空间，否则无法对齐
        updateLabelText(label1, originalText1, columnWidth - 20); // 减去图标长度
        updateLabelText(label3, originalText3, columnWidth - 5);
    }

private:
    QLabel *label1, *label3;
    TagLabel *label2;
    QString originalText1, originalText2, originalText3;
    QHBoxLayout *layout;

    // 更新 QLabel 的文本显示，适配宽度并添加省略号
    void updateLabelText(QLabel *label, const QString &originalText, int columnWidth) {
        QFontMetrics metrics(label->font());
        QString elidedText = metrics.elidedText(originalText, Qt::ElideRight, columnWidth - 10 );  // 留一点边距
        label->setText(elidedText);
    }
};


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
        emit currentChanged(currentText);
    }

signals:
    void currentChanged(const QString &text);

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
    explicit AudioChoiceDialog(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置对话框标题
        setWindowTitle("请选择音频设备");

        // 创建一个 QLabel 来描述下拉框的用途
        QLabel* label = new QLabel("请选择需要关联的音频设备:", this);

        // 创建 QComboBox 并添加选项
        mComboBox = new MacStyleComboBox(this);
        AudioDeviceList deviceList = AudioManager::GetAudioOutDeviceList();
        foreach (QString name, deviceList.keys()) {
            mComboBox->addItem(name);
        }

        // 创建按钮框，包含 "确定" 和 "取消" 按钮
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        MacStyleButton *checkButton  = new MacStyleButton("添加");
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

    // 提供一个方法来返回 QComboBox 的当前选择
    QString selectedOption() const {
        return mComboBox->currentText();
    }

private:
    QComboBox* mComboBox;
};

#endif // AUDIOCUSTOM_H
