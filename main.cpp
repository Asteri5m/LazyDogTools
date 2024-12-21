#include "LazyDogTools.h"
#include "LogHandler.h"
#include "SingleApplication.h"
#include "UAC.h"


int main(int argc, char *argv[])
{
    // 设置全局日志，初始为debug，直到加载到设置内容后修改
    qInstallMessageHandler(LogHandler::messageHandler);
    // 设置全局未处理异常过滤器
    SetUnhandledExceptionFilter(LogHandler::UnhandledExceptionFilter);

    SingleApplication a(argc, argv, "LazyDogTools-SingleApplication");
    if (a.isRunning())
    {
        a.sendMessage("Only one program instance is allowed to run.");
        return 0;
    }

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
        s.deleteLater();
    }

    LazyDogTools w;
    w.show();

    if (w.loadSetting("启动后自动隐藏") == "true")
        w.hide();
    QObject::connect(&a, SIGNAL(signalMessageAvailable(QString)), &w, SLOT(onMessageAvailable(QString)));

    return a.exec();
}
