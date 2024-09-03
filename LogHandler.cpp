#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include "LogHandler.h"


LogHandler::LogHandler()
    :logDir("Log")
{
    // 初始化日志文件
    rotateLogs();
}

LogHandler::~LogHandler()
{
    logFile.close();
}

LogHandler& LogHandler::instance()
{
    static LogHandler instance;
    return instance;
}

void LogHandler::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString functionName = LogHandler::instance().extractFunctionName(QString::fromLatin1(context.function));
    LogHandler::instance().writeLog(type, functionName, msg);
}

void LogHandler::writeLog(QtMsgType type, const QString& tag, const QString& msg)
{
    QMutexLocker locker(&mutex);

    if (logDate != QDate::currentDate()) {
        rotateLogs();
    }

    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        QString level;

        switch (type) {
        case QtDebugMsg:
            level = "DEBUG";
            break;
        case QtInfoMsg:
            level = "INFO ";
            break;
        case QtWarningMsg:
            level = "WARN ";
            break;
        case QtCriticalMsg:
            level = "ERROR";
            break;
        case QtFatalMsg:
            level = "FATAL";
            break;
        }

        QString logMessage = QString("%1 [%2] - %3 - %4\n")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                 .arg(level).arg(tag).arg(msg);

        // 输出到日志
        out << logMessage;
        out.flush();

        // 控制台输出
        setvbuf(stdout, nullptr, _IONBF, 0);
        fprintf(stdout, "%s", logMessage.toLocal8Bit().constData());
    }
}

void LogHandler::rotateLogs()
{
    logDate = QDate::currentDate();

    // 关闭当前日志文件
    if (logFile.isOpen()) {
        logFile.close();
    }

    if (!logDir.exists()) {
        logDir.mkpath(".");
    }

    // 在文件打开前轮转文件，否则无法轮转
    backupOldLogs();
    deleteOldLogs();

    QString logFileName = logDir.filePath("log.txt");
    logFile.setFileName(logFileName);

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qFatal() << "Failed to open log file:" << logFileName;
        return;
    }
}

void LogHandler::backupOldLogs()
{
    QString currentLogFileName = logDir.filePath("log.txt");

    if (QFile::exists(currentLogFileName)) {
        QFileInfo fileInfo(currentLogFileName);
        if (fileInfo.lastModified().date() != logDate) {
            QString backupName = logDir.filePath(QString("log_%1.txt").arg(fileInfo.lastModified().date().toString("yyyyMMdd")));
            QFile::rename(currentLogFileName, backupName);
        }
    }
}

void LogHandler::deleteOldLogs()
{

    QStringList logFiles = logDir.entryList(QStringList() << "log_*.txt", QDir::Files);

    for (const QString &fileName : logFiles) {
        QString logfile = logDir.filePath(fileName);
        QFileInfo fileInfo(logfile);
        if (fileInfo.lastModified().date().daysTo(QDate::currentDate()) > 30) {
            QFile::remove(logfile);
        }
    }
}

QString LogHandler::extractFunctionName(const QString& functionSignature)
{
    static const QRegularExpression FuncNameRegex(R"(\b(?:__cdecl|__stdcall|__fastcall|__thiscall|__vectorcall)\s*(\w+::\w+))");
    QRegularExpressionMatch match = FuncNameRegex.match(functionSignature);
    if (match.hasMatch()) {
        return match.captured(1); // 提取函数名部分
    }
    return "UnknownFunction";
}
