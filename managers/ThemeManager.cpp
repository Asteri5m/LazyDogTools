/**
 * @file ThemeManager.cpp
 * @author Asteri5m
 * @date 2026-05-01
 * @brief 主题管理实现 - 单例模式
 */

#include "ThemeManager.h"
#include "utils/Constants.h"
#include <QDebug>
#include <QWidget>
#include <QSqlError>


ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
    initPresets();
    initializeDatabase();
    loadSettings();
    // mUxThemeLib = LoadLibrary(L"uxtheme.dll");
    // if (mUxThemeLib) {
    //     mSetWindowCompositionAttribute = (PFN_SetWindowCompositionAttribute)
    //         GetProcAddress(mUxThemeLib, "SetWindowCompositionAttribute");
    // }
}

ThemeManager::~ThemeManager()
{
    saveSettings();

    // if (mUxThemeLib) {
    //     FreeLibrary(mUxThemeLib);
    //     mUxThemeLib = nullptr;
    // }
}

void ThemeManager::initPresets()
{
    mPresets = {
        {"默认", QColor(COLOR_PRIMARY), QColor(COLOR_ACCENT), "Mac风格蓝色"},
        {"极简灰", QColor("#607D8B"), QColor("#455A64"), "简约灰色调"},
        {"暗夜黑", QColor("#1A1A2E"), QColor("#16213E"), "深色主题"},
        {"极客绿", QColor("#00C853"), QColor("#00A844"), "科技感绿色"},
        {"活力橙", QColor("#FF6D00"), QColor("#E65100"), "活力橙色"},
        {"浪漫紫", QColor("#AA00FF"), QColor("#7B1FA2"), "紫色渐变"},
        {"清新青", QColor("#00BCD4"), QColor("#0097A7"), "青色系"},
        {"玫瑰红", QColor("#E91E63"), QColor("#C2185B"), "玫瑰色调"}
    };
}

QList<ThemeManager::ThemePreset> ThemeManager::getPresets() const
{
    return mPresets;
}


QColor ThemeManager::primaryHoverColor(int offset) const
{
    if (mPrimaryColor.name(QColor::HexRgb) == "#FFFFFF" || offset == 100) {
        return mPrimaryColor;
    }

    QColor color = mPrimaryColor.lighter(offset);
    if (color.lightness() > 245) {
        return primaryHoverColor(offset - 1);
    }

    return color;
}

// ========== 快速切换实现 ==========

void ThemeManager::switchToPreset(const QString &presetName)
{
    for (const auto &preset : mPresets) {
        if (preset.name == presetName) {
            bool changed = false;
            if (mPrimaryColor != preset.primaryColor) {
                mPrimaryColor = preset.primaryColor;
                emit primaryColorChanged(preset.primaryColor);
                changed = true;
            }
            if (mAccentColor != preset.accentColor) {
                mAccentColor = preset.accentColor;
                emit accentColorChanged(preset.accentColor);
                changed = true;
            }
            if (mThemeName != preset.name) {
                mThemeName = preset.name;
                emit themeNameChanged(preset.name);
            }
            if (changed) {
                emit themeChanged();
            }
            saveSettings();
            return;
        }
    }
    qWarning() << "未找到预设主题:" << presetName;
}

void ThemeManager::switchToPreset(int index)
{
    if (index >= 0 && index < mPresets.size()) {
        const auto &preset = mPresets.at(index);
        bool changed = false;
        if (mPrimaryColor != preset.primaryColor) {
            mPrimaryColor = preset.primaryColor;
            emit primaryColorChanged(preset.primaryColor);
            changed = true;
        }
        if (mAccentColor != preset.accentColor) {
            mAccentColor = preset.accentColor;
            emit accentColorChanged(preset.accentColor);
            changed = true;
        }
        if (mThemeName != preset.name) {
            mThemeName = preset.name;
            emit themeNameChanged(preset.name);
        }
        if (changed) {
            emit themeChanged();
        }
        saveSettings();
    }
}

void ThemeManager::switchPrimaryColor(const QColor &color)
{
    if (mPrimaryColor == color) return;
    mPrimaryColor = color;
    emit primaryColorChanged(color);
    emit themeChanged();
    saveSettings();
}

void ThemeManager::switchAccentColor(const QColor &color)
{
    if (mAccentColor == color) return;
    mAccentColor = color;
    emit accentColorChanged(color);
    emit themeChanged();
    saveSettings();
}

void ThemeManager::switchAcrylicEnabled(bool enabled)
{
    if (mAcrylicEnabled == enabled) return;
    mAcrylicEnabled = enabled;
    emit acrylicEnabledChanged(enabled);
    emit themeChanged();
    saveSettings();
}

void ThemeManager::switchAcrylicOpacity(int opacity)
{
    opacity = qBound(0, opacity, 100);
    if (mAcrylicOpacity == opacity) return;
    mAcrylicOpacity = opacity;
    emit acrylicOpacityChanged(opacity);
    emit themeChanged();
    // 延迟保存，由 SettingsWidget 在 sliderReleased 时调用 saveSettings()
}

void ThemeManager::switchCardOpacity(int opacity)
{
    opacity = qBound(0, opacity, 100);
    if (mCardOpacity == opacity) return;
    mCardOpacity = opacity;
    emit cardOpacityChanged(opacity);
    emit themeChanged();
    // 延迟保存，由 SettingsWidget 在 sliderReleased 时调用 saveSettings()
}

// ========== 设置持久化（数据库） ==========

bool ThemeManager::initializeDatabase()
{
    QDir dbDir(DIR_DB);
    if (!dbDir.exists()) dbDir.mkpath(".");

    QString dbName = DB_THEMES;
    if (QSqlDatabase::contains(dbName))
        mdb = QSqlDatabase::database(dbName);
    else
    {
        mdb = QSqlDatabase::addDatabase("QSQLITE", dbName);
        mdb.setDatabaseName(dbDir.filePath(dbName));
    }

    if (!mdb.open())
    {
        qCritical() << "Failed to open theme database:" << mdb.lastError().text();
        return false;
    }

    QSqlQuery query(mdb);
    return query.exec("CREATE TABLE IF NOT EXISTS theme_settings ("
                      "key TEXT PRIMARY KEY,"
                      "value TEXT)");
}

QString ThemeManager::loadFromDB(const QString &key, const QString &defaultValue) const
{
    if (!mdb.isOpen()) {
        qWarning() << "Theme database is not open";
        return defaultValue;
    }

    QSqlQuery query(mdb);
    query.prepare("SELECT value FROM theme_settings WHERE key = :key");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qCritical() << "Load theme setting failed:" << query.lastError().text();
        return defaultValue;
    }

    if (!query.next()) {
        // 如果没有值，保存默认值
        saveToDB(key, defaultValue);
        return defaultValue;
    }

    return query.value(0).toString();
}

bool ThemeManager::saveToDB(const QString &key, const QString &value) const
{
    if (!mdb.isOpen()) {
        qWarning() << "Theme database is not open, save failed";
        return false;
    }

    QSqlQuery query(mdb);
    query.prepare("INSERT OR REPLACE INTO theme_settings (key, value) VALUES (:key, :value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    if (!query.exec()) {
        qCritical() << "Save theme setting failed:" << query.lastError().text();
        return false;
    }

    return true;
}

void ThemeManager::loadSettings()
{
    mAcrylicEnabled = loadFromDB("acrylicEnabled", ACRYLIC_ENABLED_DEFAULT ? "true" : "false") == "true";
    mAcrylicOpacity = loadFromDB("acrylicOpacity", QString::number(OPACITY_BG_DEFAULT)).toInt();
    mCardOpacity = loadFromDB("cardOpacity", QString::number(OPACITY_CARD_DEFAULT)).toInt();

    QString primaryStr = loadFromDB("primaryColor", COLOR_PRIMARY);
    QString accentStr = loadFromDB("accentColor", COLOR_ACCENT);
    mPrimaryColor = QColor(primaryStr);
    mAccentColor = QColor(accentStr);

    mThemeName = loadFromDB("themeName", "默认");
}

void ThemeManager::saveSettings()
{
    saveToDB("acrylicEnabled", mAcrylicEnabled ? "true" : "false");
    saveToDB("acrylicOpacity", QString::number(mAcrylicOpacity));
    saveToDB("cardOpacity", QString::number(mCardOpacity));
    saveToDB("primaryColor", mPrimaryColor.name(QColor::HexArgb));
    saveToDB("accentColor", mAccentColor.name(QColor::HexArgb));
    saveToDB("themeName", mThemeName);
}
