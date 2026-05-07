#ifndef MACSTYLECOMBOBOX_H
#define MACSTYLECOMBOBOX_H

/**
 * @file MacStyleComboBox.h
 * @brief Mac样式ComboBox---自定义实现，非继承 QComboBox
 *
 * Usage:
 *     MacStyleComboBox *c = new MacStyleComboBox(parent);
 *     c->addItems({"Apple", "Banana", "Cherry"});
 *     connect(c, &MacStyleComboBox::activated, [](int idx){ qDebug() << "activated" << idx; });
 *
 * 已支持：键盘上下选择、回车确认、鼠标点击选择、popup 无系统边框与阴影、圆角+边框、可自定义 item 委托（已内置）。
 */

#include <QWidget>
#include <QStringListModel>
#include <QPainter>
#include <QRect>
#include <QPainterPath>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QProxyStyle>
#include <QScrollBar>
#include <QListView>
#include <QVBoxLayout>
#include <QSvgRenderer>
#include <QEnterEvent>

class ThemeManager;

// =====================================================
// 内部辅助类
// =====================================================

/**
 * @brief PopupWidget - 自绘的 popup 容器（无系统阴影，圆角边框）
 */
class PopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PopupWidget(QWidget *parent = nullptr);
    QListView* listView() const { return mListView; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QListView *mListView;
};

/**
 * @brief ComboBoxDelegate - ComboBox 下拉列表项委托
 */
class ComboBoxDelegate : public QStyledItemDelegate
{
public:
    explicit ComboBoxDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    mutable QSvgRenderer mCheckWhite { QLatin1String(":/ico/check_white.svg") };
    mutable QSvgRenderer mCheckBlack { QLatin1String(":/ico/check_black.svg") };
};

/**
 * @brief FloatingScrollBarStyle - 下拉列表的滚动条样式
 */
class FloatingScrollBarStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const override;
};

// =====================================================
// MacStyleComboBox 主类
// =====================================================

class MacStyleComboBox : public QWidget
{
    Q_OBJECT
public:
    explicit MacStyleComboBox(QWidget *parent = nullptr);
    explicit MacStyleComboBox(const QString &text = QString(), QWidget *parent = nullptr);

public slots:
    void onThemeChanged();

    // 设置与获取组件名，便于识别
    void setText(const QString &text){ mText = text; }
    const QString text()             { return mText; }

    // 基础API
    void addItem(const QString &text);
    void addItems(const QStringList &list);
    void clear();

    int count() const { return mModel->rowCount(); }
    QString itemText(int row) const { return mModel->data(mModel->index(row)).toString(); }

    QString currentText() const { return (mCurrentIndex >= 0 && mCurrentIndex < mModel->rowCount()) ? itemText(mCurrentIndex) : QString(); }
    int currentIndex() const { return mCurrentIndex; }

    void setCurrentIndex(int idx);
    void setCurrentText(const QString &text);

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString &text);
    void activated(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private slots:
    void onListClicked(const QModelIndex &index);

private:
    void showPopup();
    void hidePopup();
    void togglePopup();
    int calcPopupWidth() const;
    void navigate(int step);

private:
    bool mPressed { false };
    int mCurrentIndex { -1 };
    QString mText { "ComboBox" };
    PopupWidget *mPopup { nullptr };
    QStringListModel *mModel { nullptr };
};

#endif // MACSTYLECOMBOBOX_H
