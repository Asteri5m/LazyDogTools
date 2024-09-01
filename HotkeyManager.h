#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

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

    UINT MapQtKeyToVk(int key);
signals:
    void hotkeyPressed(int id);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    QMap<int, UINT> m_hotkeys; // Map of id to hotkey codes
};

#endif // HOTKEYMANAGER_H
