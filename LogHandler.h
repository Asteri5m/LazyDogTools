#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QFile>
#include <QDate>
#include <QMutex>
#include <QMutexLocker>
#include <QDir>
#include <QRegularExpression>
// #include <QTuple>


enum LogLevel {
    DebugLevel,
    InfoLevel,
    WarningLevel,
    CriticalLevel,
    FatalLevel,
    Undefined,
};




class LogHandler
{
public:
    static LogHandler& instance();
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    void writeLog(QtMsgType type, const QString& tag, const QString& msg);
    void rotateLogs();
    void setLogLevel(LogLevel level);
    LogLevel logLevel() const;
    void clearBuffer();

private:
    LogHandler();
    ~LogHandler();

    struct LogEntry {
        QtMsgType type;
        QString tag;
        QString msg;
    };

    typedef QVector<LogEntry> LogBuffer;


    bool enablePrint(QtMsgType type);
    void backupOldLogs();
    void deleteOldLogs();
    QString extractFunctionName(const QString &functionSignature);
    void bufferLog(QtMsgType type, const QString &tag, const QString &msg);

    QDir mLogDir;
    QFile mLogFile;
    QMutex mMutex;
    QDate mLogDate;
    LogLevel mLogLevel;
    LogBuffer mLogBuffer;
};

#endif // LOGHANDLER_H
