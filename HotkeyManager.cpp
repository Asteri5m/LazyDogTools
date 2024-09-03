#include "HotkeyManager.h"


HotkeyManager::HotkeyManager(QObject *parent) : QObject(parent)
{
}

HotkeyManager::~HotkeyManager()
{
    // Unregister all hotkeys
    for(auto it = m_hotkeys.begin(); it != m_hotkeys.end(); ++it)
    {
        UnregisterHotKey(nullptr, it.key());
    }
}

bool HotkeyManager::registerHotkey(int id, const QKeySequence &keySequence)
{
    // 获取修饰符
    int modifiers = keySequence[0] & (Qt::KeyboardModifierMask);
    int key = keySequence[0] & ~modifiers;

    // 将 Qt 的修饰符转换为 Windows API 的修饰符
    UINT fsModifiers = 0;
    if (modifiers & Qt::ShiftModifier)   fsModifiers |= MOD_SHIFT;
    if (modifiers & Qt::ControlModifier) fsModifiers |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier)     fsModifiers |= MOD_ALT;
    if (modifiers & Qt::MetaModifier)    fsModifiers |= MOD_WIN;

    qDebug() << keySequence << fsModifiers << key;

    // 将键位转换为 Windows 的虚拟键码
    UINT vk = MapQtKeyToVk(key);

    // 注册全局热键
    if (RegisterHotKey(nullptr, id, fsModifiers, vk))
    {
        m_hotkeys[id] = vk;
        qDebug() << "Registered hotkey:" << vk << ", Modifiers:" << fsModifiers << ", ID:" << id;
        return true;
    }
    else
    {
        qWarning() << "Failed to register hotkey:" << vk << ", Modifiers:" << fsModifiers << ", ID:" << id;
        return false;
    }
}

void HotkeyManager::unregisterHotkey(int id)
{
    if (m_hotkeys.contains(id))
    {
        UnregisterHotKey(nullptr, id);
        m_hotkeys.remove(id);
        qDebug() << "Unregistered hotkey ID:" << id;
    }
}

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result);
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY)
        {
            int id = msg->wParam;
            emit hotkeyPressed(id);
            return true;
        }
    }
    return false;
}

UINT HotkeyManager::MapQtKeyToVk(int key)
{
    switch (key)
    {
        case Qt::Key_Enter: return VK_RETURN;
        case Qt::Key_Tab: return VK_TAB;
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Escape: return VK_ESCAPE;
        case Qt::Key_Delete: return VK_DELETE;
        case Qt::Key_Insert: return VK_INSERT;
        case Qt::Key_Home: return VK_HOME;
        case Qt::Key_End: return VK_END;
        case Qt::Key_PageUp: return VK_PRIOR;
        case Qt::Key_PageDown: return VK_NEXT;
        case Qt::Key_Up: return VK_UP;
        case Qt::Key_Down: return VK_DOWN;
        case Qt::Key_Left: return VK_LEFT;
        case Qt::Key_Right: return VK_RIGHT;
        case Qt::Key_F1: return VK_F1;
        case Qt::Key_F2: return VK_F2;
        case Qt::Key_F3: return VK_F3;
        case Qt::Key_F4: return VK_F4;
        case Qt::Key_F5: return VK_F5;
        case Qt::Key_F6: return VK_F6;
        case Qt::Key_F7: return VK_F7;
        case Qt::Key_F8: return VK_F8;
        case Qt::Key_F9: return VK_F9;
        case Qt::Key_F10: return VK_F10;
        case Qt::Key_F11: return VK_F11;
        case Qt::Key_F12: return VK_F12;
        case Qt::Key_0: return '0';
        case Qt::Key_1: return '1';
        case Qt::Key_2: return '2';
        case Qt::Key_3: return '3';
        case Qt::Key_4: return '4';
        case Qt::Key_5: return '5';
        case Qt::Key_6: return '6';
        case Qt::Key_7: return '7';
        case Qt::Key_8: return '8';
        case Qt::Key_9: return '9';
        case Qt::Key_A: return 'A';
        case Qt::Key_B: return 'B';
        case Qt::Key_C: return 'C';
        case Qt::Key_D: return 'D';
        case Qt::Key_E: return 'E';
        case Qt::Key_F: return 'F';
        case Qt::Key_G: return 'G';
        case Qt::Key_H: return 'H';
        case Qt::Key_I: return 'I';
        case Qt::Key_J: return 'J';
        case Qt::Key_K: return 'K';
        case Qt::Key_L: return 'L';
        case Qt::Key_M: return 'M';
        case Qt::Key_N: return 'N';
        case Qt::Key_O: return 'O';
        case Qt::Key_P: return 'P';
        case Qt::Key_Q: return 'Q';
        case Qt::Key_R: return 'R';
        case Qt::Key_S: return 'S';
        case Qt::Key_T: return 'T';
        case Qt::Key_U: return 'U';
        case Qt::Key_V: return 'V';
        case Qt::Key_W: return 'W';
        case Qt::Key_X: return 'X';
        case Qt::Key_Y: return 'Y';
        case Qt::Key_Z: return 'Z';
        case Qt::Key_Space: return VK_SPACE;
        case Qt::Key_BracketLeft: return VK_OEM_4;  // Example for '['
        case Qt::Key_BracketRight: return VK_OEM_6; // Example for ']'
        case Qt::Key_Semicolon: return VK_OEM_1;    // Example for ';'
        case Qt::Key_Apostrophe: return VK_OEM_7;   // Example for '''
        case Qt::Key_Comma: return VK_OEM_COMMA;    // Example for ','
        case Qt::Key_Period: return VK_OEM_PERIOD;  // Example for '.'
        case Qt::Key_Slash: return VK_OEM_2;        // Example for '/'
        case Qt::Key_Backslash: return VK_OEM_5;    // Example for '\'
        case Qt::Key_Minus: return VK_OEM_MINUS;    // `-`
        case Qt::Key_Equal: return VK_OEM_PLUS;     // `=`
        default: return 0;
    }
}
