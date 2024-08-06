#ifndef LAZYDOGTOOLS_H
#define LAZYDOGTOOLS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class LazyDogTools;
}
QT_END_NAMESPACE

class LazyDogTools : public QDialog
{
    Q_OBJECT

public:
    LazyDogTools(QWidget *parent = nullptr);
    ~LazyDogTools();

private:
    Ui::LazyDogTools *ui;
};
#endif // LAZYDOGTOOLS_H
