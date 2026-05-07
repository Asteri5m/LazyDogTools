/**
 * @file ColorPickerWidget.cpp
 * @brief 颜色选择器控件实现
 */

#include "ColorPickerWidget.h"
#include "components/MacStyleButton.h"
#include "utils/Constants.h"

// =====================================================
// ColorPickerButton 实现
// =====================================================

ColorPickerButton::ColorPickerButton(const QColor &color, QWidget *parent)
    : QPushButton(parent), mColor(color)
{
    setFixedSize(40, 24);
    setCursor(Qt::PointingHandCursor);
    // setStyleSheet(QString("QPushButton { border: none; border-radius: %1px; }").arg(RADIUS_SMALL));
}

void ColorPickerButton::setColor(const QColor &color)
{
    if (mColor == color) return;
    mColor = color;
    update();
    emit colorChanged(mColor);
}

void ColorPickerButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints((QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform));

    // 绘制颜色预览区域（圆角矩形）
    QRect colorRect = rect().adjusted(2, 2, -2, -2);
    painter.setPen(Qt::NoPen);
    QPainterPath path;
    path.addRoundedRect(colorRect, RADIUS_SMALL, RADIUS_SMALL);
    painter.fillPath(path, mColor);

    // 绘制边框
    QPen pen(QColor(COLOR_WHITE));
    pen.setWidthF(1.0);
    painter.setPen(pen);
    painter.drawRoundedRect(colorRect.adjusted(1, 1, -1, -1), RADIUS_SMALL, RADIUS_SMALL);
}

void ColorPickerButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QPushButton::mousePressEvent(event);
}

// =====================================================
// ColorPickerPopup 实现
// =====================================================

ColorPickerPopup::ColorPickerPopup(QWidget *parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
{
    setFixedWidth(280);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(MARGIN_LARGE, MARGIN_LARGE, MARGIN_LARGE, MARGIN_LARGE);

    // 预设颜色
    mPresetColors = {
        // 红色系
        QColor("#FF5252"), QColor("#FF1744"), QColor("#D50000"), QColor("#C62828"),
        // 粉色系
        QColor("#FF4081"), QColor("#F50057"), QColor("#C51162"), QColor("#E91E63"),
        // 紫色系
        QColor("#E040FB"), QColor("#AA00FF"), QColor("#7B1FA2"), QColor("#9C27B0"),
        // 蓝色系
        QColor("#536DFE"), QColor("#3D5AFE"), QColor("#304FFE"), QColor("#007AFF"),
        QColor("#2196F3"), QColor("#2979FF"), QColor("#448AFF"), QColor("#1E88E5"),
        // 青色系
        QColor("#18FFFF"), QColor("#00E5FF"), QColor("#00B8D4"), QColor("#00BCD4"),
        QColor("#00ACC1"), QColor("#0097A7"), QColor("#00838F"),
        // 绿色系
        QColor("#69F0AE"), QColor("#00E676"), QColor("#00C853"), QColor("#43A047"),
        QColor("#4CAF50"), QColor("#66BB6A"), QColor("#388E3C"),
        // 黄色系
        QColor("#FFFF00"), QColor("#FFEA00"), QColor("#FFD600"), QColor("#FFAB00"),
        // 橙色系
        QColor("#FF6E40"), QColor("#FF3D00"), QColor("#FF6D00"), QColor("#FF9100"),
        // 灰度系
        QColor("#FAFAFA"), QColor("#EEEEEE"), QColor("#BDBDBD"), QColor("#9E9E9E"),
        QColor("#757575"), QColor("#616161"), QColor("#424242"), QColor("#212121")
    };

    // 标题
    QLabel *titleLabel = new QLabel("选择颜色");
    titleLabel->setStyleSheet(QString("font-weight: %1; font-size: %2px; color: " COLOR_TEXT_PRIMARY ";").arg(QFont::Medium).arg(FONT_SIZE_LARGE));
    mainLayout->addWidget(titleLabel);

    // 颜色网格
    mColorGridLayout = new QGridLayout();
    mColorGridLayout->setSpacing(4);
    setupColorGrid();
    mainLayout->addLayout(mColorGridLayout);

    // 分隔线
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border: none; background-color: " COLOR_BORDER_LIGHT ";");
    line->setFixedHeight(1);
    mainLayout->addWidget(line);

    // 自定义颜色区域
    setupCustomColor();
    mainLayout->addWidget(mCustomColorWidget);
    mainLayout->addStretch();
    setFocusPolicy(Qt::NoFocus);
}

ColorPickerPopup::~ColorPickerPopup()
{
}

void ColorPickerPopup::setupColorGrid()
{
    // 清除现有颜色按钮
    QLayoutItem *child;
    while ((child = mColorGridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    int row = 0, col = 0;
    const int maxCol = 8;

    for (int i = 0; i < mPresetColors.size(); ++i) {
        QPushButton *colorBtn = new QPushButton(this);
        colorBtn->setFixedSize(28, 28);
        colorBtn->setStyleSheet(QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: 1px solid rgba(0,0,0,0.1);"
            "    border-radius: %2px;"
            "}"
            "QPushButton:hover {"
            "    border: 2px solid %1;"
            "    border-radius: %2px;"
            "}"
        ).arg(mPresetColors[i].name()).arg(RADIUS_SMALL));

        colorBtn->setCursor(Qt::PointingHandCursor);
        connect(colorBtn, &QPushButton::clicked, this, [this, i]() {
            applyColor(mPresetColors[i]);
        });

        mColorGridLayout->addWidget(colorBtn, row, col);
        col++;
        if (col >= maxCol) {
            col = 0;
            row++;
        }
    }
}

void ColorPickerPopup::setupCustomColor()
{
    mCustomColorWidget = new QWidget();

    QVBoxLayout *customLayout = new QVBoxLayout(mCustomColorWidget);
    customLayout->setContentsMargins(0, 0, 0, 0);
    customLayout->setSpacing(8);

    // 预览 + 十六进制输入（同一行）
    QHBoxLayout *colorInputLayout = new QHBoxLayout();
    colorInputLayout->setSpacing(8);

    // 颜色预览
    QLabel *previewLabel = new QLabel("预览:", mCustomColorWidget);
    mPreviewLabel = new QLabel(mCustomColorWidget);
    mPreviewLabel->setFixedSize(60, 24);
    mPreviewLabel->setStyleSheet(QString(
        "QLabel {"
        "    background-color: %1;"
        "    border-radius: %2px;"
        "}"
    ).arg(mCurrentColor.name()).arg(RADIUS_SMALL));

    // 十六进制输入
    QLabel *hexLabel = new QLabel("HEX:", mCustomColorWidget);
    mHexInput = new QLineEdit(mCustomColorWidget);
    mHexInput->setPlaceholderText(COLOR_WHITE);
    mHexInput->setMaxLength(7);
    mHexInput->setFixedHeight(24);
    mHexInput->setText(mCurrentColor.name().toUpper());
    mHexInput->setStyleSheet(QString(
        "QLineEdit {"
        "    background-color: transparent;"
        "    border: 1px solid %1;"
        "    border-radius: %2px;"
        "    padding: 0 %3px;"
        "}"
    ).arg(COLOR_BORDER_DARK).arg(RADIUS_SMALL).arg(PADDING_SMALL));

    // 输入时实时更新预览
    connect(mHexInput, &QLineEdit::textEdited, this, [this](const QString &text) {
        if (text.startsWith("#") && (text.length() == 7 || text.length() == 9)) {
            QColor color(text);
            if (color.isValid()) {
                mPreviewLabel->setStyleSheet(QString(
                    "QLabel {"
                    "    background-color: %1;"
                    "    border-radius: %2px;"
                    "}"
                ).arg(color.name()).arg(RADIUS_SMALL));
            }
        }
    });

    // 回车时应用颜色
    connect(mHexInput, &QLineEdit::returnPressed, this, [this]() {
        QString text = mHexInput->text().trimmed();
        if (text.startsWith("#") && (text.length() == 7 || text.length() == 9)) {
            QColor color(text);
            if (color.isValid()) {
                applyColor(color);
            }
        }
    });

    colorInputLayout->addWidget(previewLabel);
    colorInputLayout->addWidget(mPreviewLabel);
    colorInputLayout->addWidget(hexLabel);
    colorInputLayout->addWidget(mHexInput);
    colorInputLayout->addStretch();
    customLayout->addLayout(colorInputLayout);

    // 更多颜色按钮
    MacStyleButton *moreColorsBtn = new MacStyleButton("更多颜色", mCustomColorWidget);
    moreColorsBtn->setNormalColorBlue(true);
    connect(moreColorsBtn, &QPushButton::clicked, this, [this]() {
        QColorDialog dialog(mCurrentColor, this);
        dialog.setWindowIcon(QIcon(":/ico/settings2.svg"));
        dialog.setOption(QColorDialog::DontUseNativeDialog);
        dialog.setOption(QColorDialog::ShowAlphaChannel);
        if (dialog.exec() == QDialog::Accepted) {
            applyColor(dialog.selectedColor());
        }
    });
    customLayout->addWidget(moreColorsBtn);
}

void ColorPickerPopup::setCurrentColor(const QColor &color)
{
    mCurrentColor = color;
    if (mHexInput) {
        mHexInput->setText(color.name().toUpper());
    }
    if (mPreviewLabel) {
        mPreviewLabel->setStyleSheet(QString(
            "QLabel {"
            "    background-color: %1;"
            "    border-radius: %2px;"
            "}"
        ).arg(color.name()).arg(RADIUS_SMALL));
    }
}

void ColorPickerPopup::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QRect r = rect().adjusted(0, 0, -1, -1);
    QPainterPath path;
    path.addRoundedRect(r, RADIUS_MEDIUM, RADIUS_MEDIUM);

    // 背景
    p.fillPath(path, Qt::white);

    // 边框
    QPen pen(QColor(200, 200, 200));
    pen.setWidthF(1.0);
    p.setPen(pen);
    p.drawPath(path);
}

void ColorPickerPopup::applyColor(const QColor &color)
{
    mCurrentColor = color;
    mHexInput->setText(color.name().toUpper());
    mPreviewLabel->setStyleSheet(QString(
        "QLabel {"
        "    background-color: %1;"
        "    border-radius: %2px;"
        "}"
    ).arg(color.name()).arg(RADIUS_SMALL));

    emit colorSelected(color);
    hide();
}

// =====================================================
// ColorPickerWidget 实现
// =====================================================

ColorPickerWidget::ColorPickerWidget(QWidget *parent)
    : QWidget(parent)
    , mColor(Qt::blue)
    , mColorButton(nullptr)
    , mTextLabel(nullptr)
    , mPopup(nullptr)
{
    init();
}

ColorPickerWidget::ColorPickerWidget(const QColor &initialColor, QWidget *parent)
    : QWidget(parent)
    , mColor(initialColor)
    , mColorButton(nullptr)
    , mTextLabel(nullptr)
    , mPopup(nullptr)
{
    init();
}

void ColorPickerWidget::init()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // 文本标签
    mTextLabel = new QLabel(this);
    mTextLabel->setStyleSheet(QString("font-size: %1px;").arg(FONT_SIZE_LARGE));
    layout->addWidget(mTextLabel);

    // 颜色按钮
    mColorButton = new ColorPickerButton(mColor, this);
    layout->addWidget(mColorButton);

    // 颜色预览标签
    mPreviewLabel = new QLabel(this);
    mPreviewLabel->setFixedSize(100, 20);
    mPreviewLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    // 根据背景色计算对比色（亮度 > 128 用黑色，否则用白色）
    QString textColor = (mColor.redF() * 0.299 + mColor.greenF() * 0.587 + mColor.blueF() * 0.114 > 0.5) ? COLOR_TEXT_PRIMARY : COLOR_WHITE;
    mPreviewLabel->setStyleSheet(QString(
        "QLabel {"
        "    background-color: %1;"
        "    border: 1px solid %2;"
        "    border-radius: %3px;"
        "    padding-left: %4px;"
        "    font-size: %5px;"
        "    color: %6;"
        "}"
    ).arg(mColor.name()).arg(COLOR_BORDER_DARK).arg(RADIUS_SMALL).arg(PADDING_MEDIUM).arg(FONT_SIZE_LARGE).arg(textColor));
    mPreviewLabel->setText(mColor.name().toUpper());
    layout->addWidget(mPreviewLabel);
    layout->addStretch();

    // 创建弹出框
    mPopup = new ColorPickerPopup(this);
    mPopup->setCurrentColor(mColor);

    // 信号连接
    connect(mColorButton, &QPushButton::clicked, this, &ColorPickerWidget::onButtonClicked);
    connect(mPopup, &ColorPickerPopup::colorSelected, this, &ColorPickerWidget::onColorSelected);
}

void ColorPickerWidget::setColor(const QColor &color)
{
    if (mColor == color) return;
    mColor = color;
    mColorButton->setColor(color);
    mPopup->setCurrentColor(color);
    // 根据背景色计算对比色
    QString textColor = (color.redF() * 0.299 + color.greenF() * 0.587 + color.blueF() * 0.114 > 0.5) ? COLOR_TEXT_PRIMARY : COLOR_WHITE;
    mPreviewLabel->setStyleSheet(QString(
        "QLabel {"
        "    background-color: %1;"
        "    border: 1px solid %2;"
        "    border-radius: %3px;"
        "    padding-left: %4px;"
        "    font-size: %5px;"
        "    color: %6;"
        "}"
        ).arg(color.name()).arg(COLOR_BORDER_DARK).arg(RADIUS_SMALL).arg(PADDING_MEDIUM).arg(FONT_SIZE_LARGE).arg(textColor));
    mPreviewLabel->setText(color.name().toUpper());
    emit colorChanged(mColor);
}

void ColorPickerWidget::setText(const QString &text)
{
    mTextLabel->setText(text);
}

void ColorPickerWidget::onButtonClicked()
{
    // 显示颜色选择弹窗
    QPoint pos = mColorButton->mapToGlobal(QPoint(0, mColorButton->height()));
    mPopup->move(pos);
    mPopup->show();
    mPopup->activateWindow();
}

void ColorPickerWidget::onColorSelected(const QColor &color)
{
    setColor(color);
}
