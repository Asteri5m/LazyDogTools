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


#endif // AUDIOCUSTOM_H
