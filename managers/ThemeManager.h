/**
 * @file ThemeManager.h
 * @author Asteri5m
 * @date 2026-05-01
 * @brief 主题管理：配色方案和亚克力效果控制（单例模式）
 */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <Windows.h>
#include "utils/Constants.h"

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static ThemeManager& instance();

    // 颜色属性
    QColor primaryColor() const { return mPrimaryColor; }
    QColor accentColor() const { return mAccentColor; }
    QString themeName() const { return mThemeName; }

    // 获取派生颜色
    QColor primaryHoverColor(int offset = 190) const;
    QColor primaryPressedHoverColor() const { return mPrimaryColor.lighter(110); }
    QColor primaryPressedColor() const { return mPrimaryColor.darker(110); }
    QColor accentHoverColor() const { return mAccentColor.lighter(110); }
    QColor accentPressedColor() const { return mAccentColor.darker(110); }

    // 亚克力效果属性
    bool acrylicEnabled() const { return mAcrylicEnabled; }
    int acrylicOpacity() const { return mAcrylicOpacity; }

    // 卡片不透明度属性
    int cardOpacity() const { return mCardOpacity; }

    // 预设主题
    struct ThemePreset {
        QString name;
        QColor primaryColor;
        QColor accentColor;
        QString description;
    };
    QList<ThemePreset> getPresets() const;

public slots:
    // 快速切换方法
    void switchToPreset(const QString &presetName);
    void switchToPreset(int index);
    void switchPrimaryColor(const QColor &color);
    void switchAccentColor(const QColor &color);
    void switchAcrylicEnabled(bool enabled);
    void switchAcrylicOpacity(int opacity);
    void switchCardOpacity(int opacity);

    void saveSettings();

signals:
    // 主题颜色变化信号
    void primaryColorChanged(const QColor &color);
    void accentColorChanged(const QColor &color);
    void themeNameChanged(const QString &name);
    void themeChanged();  // 整体主题变化

    // 亚克力效果变化信号
    void acrylicEnabledChanged(bool enabled);
    void acrylicOpacityChanged(int opacity);

    // 卡片不透明度变化信号
    void cardOpacityChanged(int opacity);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    // 禁用拷贝和赋值
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    void loadSettings();
    void initPresets();

    // 成员变量
    QColor mPrimaryColor = QColor(COLOR_PRIMARY);
    QColor mAccentColor  = QColor(COLOR_ACCENT);
    QString mThemeName = "默认";
    QList<ThemePreset> mPresets;

    int mAcrylicOpacity  = OPACITY_BG_DEFAULT;
    int mCardOpacity     = OPACITY_CARD_DEFAULT;
    bool mAcrylicEnabled = ACRYLIC_ENABLED_DEFAULT;

    // 数据库相关
    QSqlDatabase mdb;
    bool initializeDatabase();
    QString loadFromDB(const QString &key, const QString &defaultValue) const;
    bool saveToDB(const QString &key, const QString &value) const;
};

#endif // THEMEMANAGER_H
