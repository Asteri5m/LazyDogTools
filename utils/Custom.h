#ifndef CUSTOM_H
#define CUSTOM_H

/**
 * @file Custom.h
 * @author Asteri5m
 * @date 2024-02-07 20:47:57
 * @brief 通用自定义方法
 */

#include <QApplication>
#include <QHBoxLayout>
#include <QDialog>
#include <QLabel>
#include "Constants.h"
#include "components/MacStyleButton.h"
#include "widgets/SmoothScrollArea.h"

enum MessageType {
    Info,
    Warning,
    Critical
};

// 通用的显示消息框方法
inline int showMessage(QWidget *parent, const QString &title, const QString &message,
                       MessageType messageType = Info, Qt::TextFormat textFormat = Qt::AutoText,
                       const QString &button1Text = "确定", const QString &button2Text = "")
{
    QDialog dialog(parent);

    dialog.setWindowTitle(title);
    dialog.setModal(true);
    dialog.resize(520, 320); // 最大尺寸
    dialog.setStyleSheet("background-color:" COLOR_BG_CARD ";");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // =========================
    // 顶部区域
    // =========================
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *iconLabel = new QLabel();
    QStyle::StandardPixmap iconType;

    switch (messageType)
    {
    case Info:
        iconType = QStyle::SP_MessageBoxInformation;
        break;
    case Warning:
        iconType = QStyle::SP_MessageBoxWarning;
        break;
    case Critical:
        iconType = QStyle::SP_MessageBoxCritical;
        break;
    }

    iconLabel->setPixmap(QApplication::style()->standardIcon(iconType).pixmap(32, 32));
    topLayout->setSpacing(16);
    topLayout->addWidget(iconLabel, 0, Qt::AlignTop);

    // =========================
    // 滚动文本区域
    // =========================
    SmoothScrollArea *scrollArea = new SmoothScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *textLabel = new QLabel();
    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textLabel->setText(message);
    textLabel->setTextFormat(textFormat);
    textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    textLabel->setOpenExternalLinks(true);
    contentLayout->addWidget(textLabel);
    scrollArea->setWidget(contentWidget);
    topLayout->addWidget(scrollArea);
    mainLayout->addLayout(topLayout);

    // =========================
    // 按钮区域
    // =========================
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    MacStyleButton *button1 = new MacStyleButton(button1Text);
    button1->setNormalColorBlue(true);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(button1);
    QObject::connect(button1, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (!button2Text.isEmpty())
    {
        MacStyleButton *button2 = new MacStyleButton(button2Text);
        buttonLayout->addWidget(button2);
        QObject::connect(button2, &QPushButton::clicked, &dialog, &QDialog::reject);
    }

    mainLayout->addLayout(buttonLayout);
    // 调整大小
    dialog.adjustSize();
    dialog.setFixedSize(qMin(520, dialog.width() + 30), qMin(320, dialog.height()));
    return dialog.exec();
}


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


#include <QGraphicsDropShadowEffect>
// 给窗口添加阴影
inline void addDropShadowEffect(QWidget *widget)
{
    auto *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(36);
    shadow->setOffset(0, 3);
    shadow->setColor(QColor(15, 23, 42, 18));
    widget->setGraphicsEffect(shadow);
}

#endif // CUSTOM_H
