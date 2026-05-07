#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * @file Constants.h
 * @brief 常用数值宏定义
 */

// =====================================================
// 窗口尺寸相关
// =====================================================
#define WINDOW_MIN_WIDTH     800
#define WINDOW_MIN_HEIGHT    600
#define WINDOW_DEFAULT_WIDTH  800
#define WINDOW_DEFAULT_HEIGHT 480

// 侧边栏宽度
#define SIDEBAR_WIDTH        72
#define SIDEBAR_BUTTON_SIZE  56
#define SIDEBAR_ICON_SIZE    24

// =====================================================
// 间距与边距
// =====================================================
#define PADDING_SMALL        4
#define PADDING_MEDIUM       8
#define PADDING_LARGE        12
#define PADDING_XLARGE       16

#define MARGIN_TINY          4
#define MARGIN_SMALL         6
#define MARGIN_MEDIUM        8
#define MARGIN_LARGE         10
#define MARGIN_XLARGE        12

// =====================================================
// 圆角半径
// =====================================================
#define RADIUS_SMALL         4
#define RADIUS_MEDIUM        6
#define RADIUS_LARGE         8
#define RADIUS_XLARGE        12

// =====================================================
// 透明度、亚克力
// =====================================================
#define OPACITY_MIN            0
#define OPACITY_MAX            100
#define OPACITY_DEFAULT        0
#define OPACITY_BG_DEFAULT     25
#define OPACITY_CARD_DEFAULT   50

#define ACRYLIC_ENABLED_DEFAULT  false

// =====================================================
// 动画时长 (毫秒)
// =====================================================
#define ANIMATION_FAST       150
#define ANIMATION_NORMAL     200
#define ANIMATION_SLOW       300
#define ANIMATION_SCROLL     800

// =====================================================
// 控件尺寸
// =====================================================
#define BUTTON_HEIGHT_SMALL  20
#define BUTTON_HEIGHT_MEDIUM 25
#define BUTTON_HEIGHT_LARGE  30

#define COMBOBOX_HEIGHT      23
#define SLIDER_HEIGHT        20
#define SCROLLBAR_WIDTH      12

#define CHECKBOX_SIZE        14
#define HINT_LABEL_SIZE      16

// =====================================================
// 字体大小
// =====================================================
#define FONT_SIZE_SMALL      8
#define FONT_SIZE_MEDIUM     10
#define FONT_SIZE_LARGE      12
#define FONT_SIZE_XLARGE     14

// =====================================================
// 其他
// =====================================================
#define SEPARATOR_WIDTH      1
#define BORDER_WIDTH         1

#define MAX_VISIBLE_ROWS     9

// =====================================================
// 字体
// =====================================================
#define FONT_FAMILY_PRIMARY  "MiSans"     // 主字体
#define FONT_FAMILY_DEFAULT  "Microsoft YaHei UI"


// =====================================================
// 主题颜色
// =====================================================
#define COLOR_PRIMARY        "#2F7CF6"  // 主色（蓝色）
#define COLOR_ACCENT         "#1F6AE6"  // 强调色

// =====================================================
// 文字颜色
// =====================================================
#define COLOR_TEXT_PRIMARY   "#111827"   // 主要文字
#define COLOR_TEXT_SECONDARY "#6B7280"   // 次要文字（提示文字）
#define COLOR_TEXT_DISABLED  "#AAAAAA"   // 禁用文字
#define COLOR_TEXT_SELECTED  "#FFFFFF"   // 选中文字

// =====================================================
// 背景颜色
// =====================================================
#define COLOR_BG_LIGHT       "#F9F9F9"   // 浅色背景
#define COLOR_BG_WIDGET      "#F6F7FB"   // 窗口背景
#define COLOR_BG_CARD        "#FDFDFD"   // 卡片背景

// =====================================================
// 边框颜色
// =====================================================
#define COLOR_BORDER         "#EAEDF4"   // 边框色
#define COLOR_BORDER_LIGHT   "#F0F0F0"   // 浅边框
#define COLOR_BORDER_DARK    "#DDDDDD"   // 深边框

// =====================================================
// 分隔线颜色
// =====================================================
#define COLOR_SEPARATOR      "#DEDEDE"  // 分隔线
#define COLOR_SEPARATOR_DARK "#DADADA"  // 深分隔线

// =====================================================
// 开关/滑块颜色
// =====================================================
#define COLOR_SWITCH_OFF     "#BFBFBF"  // 开关关闭状态
#define COLOR_THUMB          "#FFFFFF"  // 滑块/开关滑块颜色

// =====================================================
// 链接颜色
// =====================================================
#define COLOR_LINK           "#2D8CFF"  // 超链接颜色

// =====================================================
// 其他常用颜色
// =====================================================
#define COLOR_BUTTON_BORDER  "#CDCDCD"  // 按钮边框色
#define COLOR_TRACK          "#CCCCCC"  // 滑块轨道色
#define COLOR_WHITE          "#FFFFFF"  // 白色

// =====================================================
// 存储相关
// =====================================================
#define DIR_DB           "data"
#define DIR_LOG          "log"
#define DB_SETTINGS      "Settings.db"
#define DB_AUDIOHELPER   "AudioHelper.db"
#define DB_THEMES        "Theme.db"

#endif // CONSTANTS_H
