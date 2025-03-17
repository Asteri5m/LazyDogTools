#ifndef LAZYDOGTOOLS_H
#define LAZYDOGTOOLS_H

/**
 * @file LazyDogTools.h
 * @author Asteri5m
 * @date 2025-02-07 15:44:32
 * @brief 程序主类
 */

#include <QObject>
#include "Settings.h"

class LazyDogTools : public QObject
{
    Q_OBJECT

public:
    LazyDogTools(QObject *parent = nullptr);
    ~LazyDogTools();

public slots:
    void onMessageAvailable(QString);

private:
    Settings *mSettings;

    void initTools();
    void initTray();

private slots:
    void trayUpdate();
};
#endif // LAZYDOGTOOLS_H
