#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QFile>
#include <QDate>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QObject>
#include <QDir>
#include <QRegularExpression>


class LogHandler: public QObject
{
    Q_OBJECT
public:
    static LogHandler& instance();
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    void writeLog(QtMsgType type, const QString& tag, const QString& msg);
    void rotateLogs();

private:
    LogHandler();
    ~LogHandler();

    void backupOldLogs();
    void deleteOldLogs();
    QString extractFunctionName(const QString &functionSignature);


    QDir logDir;
    QFile logFile;
    QMutex mutex;
    QDate logDate;
    QTimer rotationTimer;
};

#endif // LOGHANDLER_H
