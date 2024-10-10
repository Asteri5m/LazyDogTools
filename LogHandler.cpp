#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
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
        mLogFile.seek(mLogFile.size());
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
            QMessageBox::critical(nullptr, "程序崩溃", msg);
            break;
        }

        QString logMessage = QString("%1 [%2] - %3 - %4\n")
                                 .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
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

LONG WINAPI LogHandler::UnhandledExceptionFilter(EXCEPTION_POINTERS *exceptionInfo) {
    // 获取异常信息
    PEXCEPTION_RECORD exceptionRecord = exceptionInfo->ExceptionRecord;

    // 输出异常代码和地址
    DWORD exceptionCode = exceptionRecord->ExceptionCode;
    PVOID exceptionAddress = exceptionRecord->ExceptionAddress;

    // 信息输出到日志
    qCritical() << "Unhandled Exception occurred!";
    qCritical() << "Exception Code:" << QString::number(exceptionCode, 16);
    qCritical() << "Exception Address:" << reinterpret_cast<quintptr>(exceptionAddress);

    // 获取线程和进程信息
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    DWORD threadId = GetThreadId(thread);
    DWORD processId = GetProcessId(process);

    qDebug() << "Thread ID:" << threadId;
    qDebug() << "Process ID:" << processId;

    // 堆栈信息
    SymInitialize(process, NULL, TRUE);
    SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    STACKFRAME64 stackFrame = {0};
    CONTEXT* context = exceptionInfo->ContextRecord;
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

    while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &stackFrame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
        if (stackFrame.AddrPC.Offset == 0)
            break;

        char symbolBuffer[sizeof(SYMBOL_INFO) + 256] = {0};
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 255;

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, 0, symbol)) {
            qDebug() <<  QString("0x%1: %2").arg(symbol->Address, 0, 16).arg(symbol->Name);
        }
    }

    QString message = QString("程序发生了意外错误，程序将关闭。<br><br>"
                              "异常代码: 0x%1<br>"
                              "异常地址: %2")
                          .arg(QString::number(exceptionCode, 16))
                          .arg(reinterpret_cast<quintptr>(exceptionAddress), 0, 16);
    qFatal() << message.toUtf8().constData();

    return EXCEPTION_EXECUTE_HANDLER;
}
