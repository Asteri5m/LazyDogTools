#ifndef CUSTOM_H
#define CUSTOM_H

/**
 * @file Custom.h
 * @author Asteri5m
 * @date 2024-02-07 20:47:57
 * @brief 通用自定义方法
 */

#include <QMessageBox>
#include "CustomWidget.h"

enum MessageType {
    Info,
    Warning,
    Critical
};

// 通用的显示消息框方法
inline int showMessage(QWidget *parent, const QString &title, const QString &message,
                       MessageType messageType = Info,
                       const QString &button1Text = "确定", const QString &button2Text = "")
{
    // 创建消息框
    QMessageBox msgBox(parent);

    // 设置消息框的图标
    switch (messageType) {
    case Info:
        msgBox.setIcon(QMessageBox::Information);
        break;
    case Warning:
        msgBox.setIcon(QMessageBox::Warning);
        break;
    case Critical:
        msgBox.setIcon(QMessageBox::Critical);
        break;
    }

    msgBox.setWindowTitle(title);      // 设置窗口标题
    msgBox.setText(message);           // 设置提示消息内容

    // 添加按钮
    MacStyleButton *button1 = new MacStyleButton(button1Text, &msgBox);
    button1->setNormalColorBlue(true);
    msgBox.addButton(button1, QMessageBox::AcceptRole);  // 第一个按钮
    if (!button2Text.isEmpty()) {
        MacStyleButton *button2 = new MacStyleButton(button2Text, &msgBox);
        msgBox.addButton(button2, QMessageBox::RejectRole);  // 第二个按钮
    }

    // 显示消息框  不知道什么原因，接受返回2，拒绝返回3……
    if (msgBox.exec() == 2)
        return QMessageBox::Accepted;
    return  QMessageBox::Rejected;
}


#include <QTableWidgetItem>
#include <QHeaderView>

class TableWidget : public QTableWidget {
    Q_OBJECT
public:
    using QTableWidget::QTableWidget;

    explicit TableWidget(QWidget *parent = nullptr) :
        QTableWidget(parent) {

        setSelectionMode(QAbstractItemView::SingleSelection); //单选
        setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
        setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应列宽

        setFrameShape(QFrame::NoFrame); //设置无边框
        setShowGrid(false); //设置不显示格子线
        verticalHeader()->setVisible(false); //设置垂直头不可见
        setFocusPolicy(Qt::NoFocus); //去除选中虚线框
        setWordWrap(false);  // 禁用自动换行

        setStyleSheet(
            "QTableWidget {"
            "   border-radius: 8px;"
            "}"
            "QTableView::item:hover {"
            "   background-color: #f0f0f2;"
            "}"
            "QTableView::item:selected {"
            "   background-color: #DEF2FB;"
            "}"
            "QHeaderView::section {"
            "   font-weight: bold;" // 设置字体加粗
            "   font-size: 14px;"
            "}"
            );
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QTableWidget::paintEvent(event);

        if (horizontalHeader()->count() == 0 || verticalHeader()->count() == 0)
            return;

        QPainter painter(viewport());

        // 获取当前样式的 QStyleOptionViewItem
        QStyleOptionViewItem option;
        option.initFrom(this);  // 使用 initFrom 替代 init

        // 设置网格线的颜色为灰色
        const QColor gridColor = QColor(236, 236, 236);  // 灰色
        QPen gridPen(gridColor, 0, Qt::SolidLine);  // 设置网格线的画笔
        painter.setPen(gridPen);

        int w = horizontalHeader()->offset();
        for (int i = 0; i < horizontalHeader()->count()-1; ++i) {
            w += horizontalHeader()->sectionSize(i);
            painter.drawLine(w - 1, 0, w - 1, viewport()->height());  // 绘制网格线
        }
    }
};

#include <QObject>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QPainter>
#include <QStyleOptionViewItem>

class TableRowDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableRowDelegate(QTableWidget *tableWidget, QObject *parent = nullptr) :
        QStyledItemDelegate(parent),
        mTableWidget(tableWidget) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if(option.state.testFlag(QStyle::State_MouseOver))
        {
            QTableWidgetItem *hoverItem = mTableWidget->item(index.row(), index.column());
            if(hoverItem != nullptr)
            {
                int row = hoverItem->row();
                int columnCount = mTableWidget->columnCount();
                for(int i = 0; i < columnCount; i++)
                {
                    QTableWidgetItem *item = mTableWidget->item(row, i);
                    if(item != nullptr)
                    {
                        QModelIndex index = mTableWidget->model()->index(row, i);
                        QStyleOptionViewItem op = option;
                        op.index = index;
                        //获取每个item对应的可视矩形
                        op.rect = mTableWidget->visualItemRect(item);
                        QStyledItemDelegate::paint(painter, op, index);
                    }
                }
            }
            return;
        }
        return QStyledItemDelegate::paint(painter, option, index);
    }

private:
    //指明被代理的tablewidget,方便获取整行的item
    QTableWidget *mTableWidget;

};


template<typename K, typename V>
class OrderedMap {
public:
    void insert(const K &key, const V &value) {
        if (!hash.contains(key)) { // 确保唯一性
            hash[key] = value;
            order.append(key);
        }
    }

    bool contains(const K &key) const {
        return hash.contains(key);
    }

    V value(const K &key) const {
        return hash.value(key);
    }

    V& operator[](const K &key) {
        return hash[key]; // 返回引用以允许修改
    }

    void remove(const K &key) {
        if (hash.remove(key)) {
            order.removeAll(key);
        }
    }

    void clear() {
        hash.clear();
        order.clear();
    }

    QList<K> keys() const {
        return order;
    }

    QList<V> values() const {
        QList<V> vals;
        for (const K &key : order) {
            vals.append(hash.value(key));
        }
        return vals;
    }

    // 提供迭代器支持
    using iterator = typename QList<K>::iterator;
    using const_iterator = typename QList<K>::const_iterator;

    iterator begin() { return order.begin(); }
    iterator end() { return order.end(); }
    const_iterator begin() const { return order.begin(); }
    const_iterator end() const { return order.end(); }


private:
    QHash<K, V> hash;    // 存储键值对
    QList<K> order;      // 存储插入顺序
};

#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

// 获取堆栈信息的辅助函数
inline QString getStackTrace()
{
    QString stackTrace;
    HANDLE process = GetCurrentProcess();
    
    // 初始化符号处理器
    SymInitialize(process, NULL, TRUE);
    
    void* stack[100];  // 最多获取100层堆栈
    WORD frames = CaptureStackBackTrace(0, 100, stack, NULL);
    
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    
    for(WORD i = 0; i < frames; i++)
    {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, 0, symbol);
        
        DWORD displacement;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        
        if(SymGetLineFromAddr64(process, address, &displacement, &line))
        {
            stackTrace += QString("\t%1() at %2:%3\n")
                .arg(QString::fromLocal8Bit(symbol->Name))
                .arg(QString::fromLocal8Bit(line.FileName))
                .arg(line.LineNumber);
        }
        else
        {
            stackTrace += QString("\t%1() at 0x%2\n")
                .arg(QString::fromLocal8Bit(symbol->Name))
                .arg(QString::number(symbol->Address, 16));
        }
    }
    
    free(symbol);
    return stackTrace;
}

#endif // CUSTOM_H
