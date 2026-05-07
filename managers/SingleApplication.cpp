#include "SingleApplication.h"
#include <QLocalSocket>

SingleApplication::SingleApplication(int& argc, char* argv[], const QString uniqueKey)
    : QApplication(argc, argv)
    , mUniqueKey(uniqueKey)
{
    mSharedMemory.setKey(mUniqueKey);
    if (mSharedMemory.attach())
    {
        mIsRunning = true;
    }
    else
    {
        mIsRunning = false;
        // create shared memory.
        if (!mSharedMemory.create(1))
        {
            qCritical("Unable to create single instance.");
            return;
        }

        // create local server and listen to incoming messages from other instances.
        mLocalServer = new QLocalServer(this);
        mLocalServer->setSocketOptions(QLocalServer::WorldAccessOption);
        connect(mLocalServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
        mLocalServer->listen(mUniqueKey);
    }
}

// public slots.
void SingleApplication::onNewConnection()
{
    QLocalSocket* localSocket = mLocalServer->nextPendingConnection();
    if (!localSocket->waitForReadyRead(mTimeout))
    {
        qCritical(localSocket->errorString().toLatin1());
        return;
    }

    QByteArray byteArray = localSocket->readAll();
    QString message = QString::fromUtf8(byteArray.constData());
    emit signalMessageAvailable(message);
    localSocket->disconnectFromServer();
}

// public functions.
bool SingleApplication::isRunning()
{
    return mIsRunning;
}

bool SingleApplication::sendMessage(const QString& message)
{
    if (!mIsRunning)
        return false;

    QLocalSocket localSocket(this);
    localSocket.connectToServer(mUniqueKey, QIODevice::WriteOnly);
    if (!localSocket.waitForConnected(mTimeout))
    {
        qCritical(localSocket.errorString().toLatin1());
        return false;
    }

    localSocket.write(message.toUtf8());
    if (!localSocket.waitForBytesWritten(mTimeout))
    {
        qCritical(localSocket.errorString().toLatin1());
        return false;
    }

    localSocket.disconnectFromServer();
    return true;
}

