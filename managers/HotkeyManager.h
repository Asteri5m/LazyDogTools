#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

/**
 * @file HotkeyManager.cpp
 * @author Asteri5m
 * @date 2025-02-07 20:33:02
 * @brief 热键管理器
 */

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QAbstractNativeEventFilter>
#include <QKeySequence>
#include <windows.h>

class HotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();

    bool registerHotkey(int id, const QKeySequence &keySequence);
    void unregisterHotkey(int id);

    UINT mapQtKeyToVk(int key);
signals:
    void hotkeyPressed(int id);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    QMap<int, UINT> m_hotkeys; // Map of id to hotkey codes
};

#endif // HOTKEYMANAGER_H
