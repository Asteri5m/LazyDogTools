#ifndef LAZYDOGTOOLS_H
#define LAZYDOGTOOLS_H

#include <QDialog>
#include <QMetaObject>
#include <QMetaMethod>
#include "ToolManager.h"
#include "Settings.h"

// 主页工具列表的基本元素
struct MinToolListItem {
    short     id;
    QString icon;
    QString name;
    QString description;
    ToolManager*   tool; //作为联系项

    // MinToolListItem(short id, const QString &icon, const QString &name, const QString &description, const QMetaObject* object)
    //     : id(id), icon(icon), name(name), description(description), type_id(std::type_index) {}
};

// 工具表
typedef QVector<ToolManager*> ToolList;
// 窗口列表
typedef QVector<MinToolListItem> MinToolList;

QT_BEGIN_NAMESPACE
namespace Ui {
class LazyDogTools;
}
QT_END_NAMESPACE

class LazyDogTools : public QDialog {
    Q_OBJECT

public:
    LazyDogTools(QWidget *parent = nullptr);
    ~LazyDogTools();

private:
    Ui::LazyDogTools *ui;

    ToolList    mToolList;
    MinToolList mMinToolList;
    QVBoxLayout *mLayout;
    QWidget     *mWidget;

    void initUI();
    void initData();
    void updateUI();
    void updateData();
    void minToolWidgetTest();
    void toolWidgetModelTest();
};



#endif // LAZYDOGTOOLS_H
