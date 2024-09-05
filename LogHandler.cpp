#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include "LogHandler.h"


LogHandler::LogHandler()
    :mLogDir("Log")
    ,mLogLevel(Undefined)
{
    // 初始化日志文件
    rotateLogs();

    setvbuf(stdout, nullptr, _IONBF, 0);
}

LogHandler::~LogHandler()
{
    // 意外终止时输出缓冲区
    if (!mLogBuffer.isEmpty()){
        LogHandler::instance().setLogLevel(DebugLevel);
        LogHandler::instance().clearBuffer();
    }

    mLogFile.close();
}

LogHandler& LogHandler::instance()
{
    static LogHandler instance;
    return instance;
}

void LogHandler::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString functionName = LogHandler::instance().extractFunctionName(QString::fromLatin1(context.function));

    if (LogHandler::instance().logLevel() == Undefined)
        LogHandler::instance().bufferLog(type, functionName, msg);

    if (!LogHandler::instance().enablePrint(type))
        return;

    LogHandler::instance().writeLog(type, functionName, msg);
}

void LogHandler::writeLog(QtMsgType type, const QString& tag, const QString& msg)
{
    QMutexLocker locker(&mMutex);

    if (mLogDate != QDate::currentDate()) {
        rotateLogs();
    }

    if (mLogFile.isOpen()) {
        QTextStream out(&mLogFile);
        QString levelText;

        switch (type) {
        case QtDebugMsg:
            levelText = "DEBUG";
            break;
        case QtInfoMsg:
            levelText = "INFO ";
            break;
        case QtWarningMsg:
            levelText = "WARN ";
            break;
        case QtCriticalMsg:
            levelText = "ERROR";
            break;
        case QtFatalMsg:
            levelText = "FATAL";
            break;
        }

        QString logMessage = QString("%1 [%2] - %3 - %4\n")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                 .arg(levelText).arg(tag).arg(msg);

        // 输出到日志
        out << logMessage;
        out.flush();

        // 控制台输出
        fprintf(stdout, "%s", logMessage.toLocal8Bit().constData());
    }
}

void LogHandler::rotateLogs()
{
    mLogDate = QDate::currentDate();

    // 关闭当前日志文件
    if (mLogFile.isOpen()) {
        mLogFile.close();
    }

    if (!mLogDir.exists()) {
        mLogDir.mkpath(".");
    }

    // 在文件打开前轮转文件，否则无法轮转
    backupOldLogs();
    deleteOldLogs();

    QString logFileName = mLogDir.filePath("log.txt");
    mLogFile.setFileName(logFileName);

    if (!mLogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qFatal() << "Failed to open log file:" << logFileName;
        return;
    }
}

void LogHandler::backupOldLogs()
{
    QString currentLogFileName = mLogDir.filePath("log.txt");

    if (QFile::exists(currentLogFileName)) {
        QFileInfo fileInfo(currentLogFileName);
        if (fileInfo.lastModified().date() != mLogDate) {
            QString backupName = mLogDir.filePath(QString("log_%1.txt").arg(fileInfo.lastModified().date().toString("yyyyMMdd")));
            QFile::rename(currentLogFileName, backupName);
        }
    }
}

void LogHandler::deleteOldLogs()
{

    QStringList logFiles = mLogDir.entryList(QStringList() << "log_*.txt", QDir::Files);

    for (const QString &fileName : logFiles) {
        QString logfile = mLogDir.filePath(fileName);
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
    return "LogHandler::Default";
}

void LogHandler::setLogLevel(LogLevel level)
{
    mLogLevel = level;
}

LogLevel LogHandler::logLevel() const
{
    return mLogLevel;
}

void LogHandler::bufferLog(QtMsgType type, const QString& tag, const QString& msg)
{
    // 将日志信息存储在缓冲区中
    mLogBuffer.append({ type, tag, msg });
}

void LogHandler::clearBuffer()
{
    // 输出缓冲区中的日志
    for (const auto& logEntry : mLogBuffer) {
        const QtMsgType type = logEntry.type;
        const QString& tag   = logEntry.tag;
        const QString& msg   = logEntry.msg;

        if (LogHandler::instance().enablePrint(type)) {
            LogHandler::instance().writeLog(type, tag, msg);
        }
    }

    mLogBuffer.clear();
}

bool LogHandler::enablePrint(QtMsgType type)
{
    LogLevel logLevel;

    switch (type) {
    case QtDebugMsg:
        logLevel = DebugLevel;
        break;
    case QtInfoMsg:
        logLevel = InfoLevel;
        break;
    case QtWarningMsg:
        logLevel = WarningLevel;
        break;
    case QtCriticalMsg:
        logLevel = CriticalLevel;
        break;
    case QtFatalMsg:
        logLevel = FatalLevel;
        break;
    }

    // 返回是否可以输出日志
    return logLevel >= LogHandler::instance().logLevel();
}
