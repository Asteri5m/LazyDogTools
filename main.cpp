#include "LazyDogTools.h"
#include "LogHandler.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置自定义消息处理器---日志管理器
    qInstallMessageHandler(LogHandler::messageHandler);
    LazyDogTools w;
    w.show();
    return a.exec();
}
