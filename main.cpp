#include "LazyDogTools.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LazyDogTools w;
    w.show();
    return a.exec();
}
