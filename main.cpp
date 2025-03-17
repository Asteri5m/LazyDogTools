#include "LazyDogTools.h"
#include "LogHandler.h"
#include "SingleApplication.h"
#include "UAC.h"
#include "Settings.h"
#include "Custom.h"
#include <QProcess>


int main(int argc, char *argv[])
{
    try {
        // 设置全局日志，初始为debug，直到加载到设置内容后修改
        qInstallMessageHandler(LogHandler::messageHandler);
        // 设置全局未处理异常过滤器
        SetUnhandledExceptionFilter(LogHandler::UnhandledExceptionFilter);

        SingleApplication a(argc, argv, "LazyDogTools-SingleApplication");

        // 检查启动参数
        QStringList args = QApplication::arguments();
        bool isStartup = args.contains("-startup");
        bool isUpdate = args.contains("-update");
        bool isClear = args.contains("-clear");

        if (isStartup) return UAC::setApplicationStartup(true, true) ? 0 : 1;
        if (isUpdate) return Settings::updateApp() ? 0 : 1;
        if (isClear)
        {
            Settings::clearUpdate();
            Sleep(1000); // 等待更新进程结束
        }

        if (a.isRunning())
        {
            a.sendMessage("Only one program instance is allowed to run.");
            return 0;
        }

        { // 限制作用域
            Settings s;
            // 检查是否需要管理员权限启动
            if (s.loadConfig("管理员模式启动") == "true" && !UAC::isRunAsAdmin())
            {
                if (UAC::runAsAdmin())
                {
                    qDebug() << "正在以管理员权限启动程序";
                    return 0; // 当前普通权限的程序实例退出
                }
                else
                    qWarning() << "管理员权限启动失败，将以普通权限继续运行";
            }
            s.deleteLater();
        }

        LazyDogTools w;
        QObject::connect(&a, SIGNAL(signalMessageAvailable(QString)), &w, SLOT(onMessageAvailable(QString)));
        QApplication::setQuitOnLastWindowClosed(false);
        return a.exec();
    }
    catch (const std::exception& e) {
        qCritical() << "捕获到异常:" << e.what();
        qCritical() << "堆栈信息:\n" << getStackTrace();
        
        // 重新启动应用程序
        QProcess::startDetached(QCoreApplication::applicationFilePath());
        return 1;
    }
    catch (...) {
        qCritical() << "捕获到未知异常";
        qCritical() << "堆栈信息:\n" << getStackTrace();
        
        // 重新启动应用程序
        QProcess::startDetached(QCoreApplication::applicationFilePath());
        return 1;
    }
}
