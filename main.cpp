#include "LazyDogTools.h"
#include "LogHandler.h"
#include "UAC.h"
#include <QApplication>
#include <QSqlQuery>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置全局日志，初始为debug，直到加载到设置内容后修改
    qInstallMessageHandler(LogHandler::messageHandler);

    // 设置全局未处理异常过滤器
    SetUnhandledExceptionFilter(LogHandler::UnhandledExceptionFilter);

    { // 限制作用域
        Settings s;
        // 以管理员权限启动一个新实例
        if (s.loadSetting("管理员模式启动") == "true")
        {
            if (UAC::runAsAdmin())
            {
                qDebug() << "管理员模式启动";
                return 0; // 启动成功，当前程序退出
            } // 未启动，当前程序继续
        }
    }

    LazyDogTools w;
    w.show();

    if (w.loadSetting("启动后自动隐藏") == "true")
        w.hide();

    return a.exec();
}
