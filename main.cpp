#include "LazyDogTools.h"
#include "LogHandler.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置全局日志，初始为debug，直到加载到设置内容后修改
    qInstallMessageHandler(LogHandler::messageHandler);

    // 设置全局未处理异常过滤器
    SetUnhandledExceptionFilter(LogHandler::UnhandledExceptionFilter);

    LazyDogTools w;
    w.show();
    return a.exec();
}
