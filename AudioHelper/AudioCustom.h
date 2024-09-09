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

        // 添加到布局
        layout->addWidget(label1);
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

        // 动态调整每个 QLabel 的文本长度并添加省略号
        updateLabelText(label1, originalText1, columnWidth);
        updateLabelText(label3, originalText3, columnWidth);
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

#include <QLineEdit>
#include <QListView>
#include <QFileSystemModel>
#include <QDir>
#include <QMessageBox>
#include <QStringListModel>

class DiskWidget : public QWidget {
    Q_OBJECT

public:
    DiskWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        // 创建导航栏
        pathListView = new QListView(this);
        pathLineEdit = new QLineEdit(this);

        pathListView->setFlow(QListView::LeftToRight);
        pathListView->setWrapping(false);
        pathListView->setSelectionMode(QListView::SingleSelection);
        pathListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // pathListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        layout->addWidget(pathListView);
        layout->addWidget(pathLineEdit);

        pathListView->setFixedHeight(30);
        pathLineEdit->setFixedHeight(30);
        pathLineEdit->hide();

        // 设置自定义委托
        pathListView->setItemDelegate(new PathDelegate(this));

        // 路径模型
        pathModel = new QStringListModel(this);
        pathListView->setModel(pathModel);

        // 文件视图：QListView 显示当前文件夹内容
        listView = new QListView(this);
        layout->addWidget(listView);

        // 文件系统模型
        fileModel = new QFileSystemModel(this);
        fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
        listView->setModel(fileModel);

        // 初始显示根目录 QDir::rootPath()
        setDirectory("");

        // 连接信号槽：点击路径列表项时切换到对应目录
        connect(pathListView, &QListView::clicked, this, &DiskWidget::onPathItemClicked);

        // 双击进入文件夹
        connect(listView, &QListView::doubleClicked, this, &DiskWidget::onItemDoubleClicked);

        connect(listView, &QListView::clicked, this, &DiskWidget::onItemClicked);

        // 双击路径列表的空白处以手动输入路径
        pathListView->viewport()->installEventFilter(this);
        // 连接信号槽: 当路径编辑框按回车键时，跳转到输入的路径
        connect(pathLineEdit, &QLineEdit::returnPressed, this, &DiskWidget::onPathEntered);
        connect(pathLineEdit, &QLineEdit::editingFinished, this, &DiskWidget::onEditingFinished);

    }

protected:
    // 捕捉双击事件，在路径栏双击空白处时，弹出输入框
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (obj == pathListView->viewport() && event->type() == QEvent::MouseButtonDblClick) {
            pathLineEdit->setText(currentPath);
            pathListView->hide();
            pathLineEdit->show();
            pathLineEdit->setFocus();
        }
        return QWidget::eventFilter(obj, event);
    }

private slots:
    // 用户点击路径中的某一部分时，切换到该路径
    void onPathItemClicked(const QModelIndex &index) {
        QStringList pathList = currentPath.split("/", Qt::SkipEmptyParts);
        QString newPath = pathList.mid(0, index.row() + 1).join("/");
        setDirectory(newPath);
    }

    // 用户双击文件夹时进入该目录
    void onItemDoubleClicked(const QModelIndex &index) {
        if (fileModel->isDir(index)) {
            QString newPath = fileModel->filePath(index);
            setDirectory(newPath);
        }
    }

    void onEditingFinished() {
        pathLineEdit->hide();
        pathListView->show();
    }

    // 用户按回车输入路径时的处理函数
    void onPathEntered() {
        QString enteredPath = pathLineEdit->text();
        pathLineEdit->hide();
        pathListView->show();
        setDirectory(enteredPath.replace("\\", "/"));
    }

    void onItemClicked(const QModelIndex &index) {
        QString currentText = fileModel->filePath(index);
        emit currentChanged(currentText);
    }

signals:
    void currentChanged(const QString &text);

private:
    // 更新当前显示的路径
    void setDirectory(const QString &path) {
        QDir dir(path);
        if (!dir.exists()) {
            QMessageBox::warning(this, tr("路径错误"), tr("该路径不存在！"));
        } else {
            currentPath = path;

            // 更新路径模型
            QStringList pathList = currentPath.split("/", Qt::SkipEmptyParts);
            pathModel->setStringList(pathList);

            // 始终滚动到路径的最后一个部分
            pathListView->scrollTo(pathModel->index(pathModel->rowCount() - 1), QAbstractItemView::PositionAtCenter);

            // 更新文件视图
            listView->setRootIndex(fileModel->setRootPath(currentPath));
        }
    }

    QLineEdit *pathLineEdit;
    QListView *pathListView;          // 用于显示路径的横向列表
    QStringListModel *pathModel;      // 路径模型
    QListView *listView;              // 文件列表视图
    QFileSystemModel *fileModel;      // 文件系统模型
    QString currentPath;              // 当前路径
};

#endif // AUDIOCUSTOM_H
