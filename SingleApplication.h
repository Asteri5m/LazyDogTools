#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

/*!
 * \file SingleApplication.h
 *
 * \author Kandy
 * \date 2022/3/17 15:21:16
 *
 * \brief 程序单例
 * \link https://blog.csdn.net/hellokandy/article/details/124195848
 */
#pragma once
#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int& argc, char* argv[], const QString uniqueKey);
    bool isRunning();
    bool sendMessage(const QString& message);

signals:
    void signalMessageAvailable(QString message);

public slots:
    void onNewConnection();

private:
    bool mIsRunning;
    QString mUniqueKey;
    QSharedMemory mSharedMemory;
    QLocalServer* mLocalServer;
    static const int mTimeout = 1000;
};


#endif // SINGLEAPPLICATION_H
