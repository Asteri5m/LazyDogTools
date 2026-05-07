/**
 * @file MacStyleComboBox.cpp
 * @brief Mac样式ComboBox---自定义实现
 */

#include "MacStyleComboBox.h"
#include "managers/ThemeManager.h"
#include "utils/Constants.h"
#include "utils/Custom.h"

// =====================================================
// PopupWidget 实现
// =====================================================

PopupWidget::PopupWidget(QWidget *parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    mListView = new QListView(this);
    mListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    mListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mListView->setSelectionMode(QAbstractItemView::SingleSelection);
    mListView->setUniformItemSizes(true);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setContentsMargins(RADIUS_MEDIUM, RADIUS_MEDIUM, RADIUS_MEDIUM, RADIUS_MEDIUM);
    l->setSpacing(0);
    l->addWidget(mListView);
    setLayout(l);

    setFocusPolicy(Qt::NoFocus);
}

void PopupWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QRect r = rect().adjusted(0, 0, -1, -1);
    QPainterPath path;
    path.addRoundedRect(r, RADIUS_MEDIUM, RADIUS_MEDIUM);

    p.fillPath(path, Qt::white);

    QPen pen(QColor(COLOR_BORDER_DARK));
    pen.setWidthF(1.0);
    p.setPen(pen);
    p.drawPath(path);
}

// =====================================================
// ComboBoxDelegate 实现
// =====================================================

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRect baseRect = option.rect.adjusted(5, 1, -5, -1);
    bool isHovered = option.state & QStyle::State_MouseOver;
    bool isSelected = option.state & QStyle::State_Selected;

    QRect checkRect(baseRect.left() + 3, baseRect.top() + (baseRect.height() - 14)/2, 14, 14);

    if (isHovered) {
        painter->setBrush(ThemeManager::instance().primaryColor());
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(baseRect, RADIUS_SMALL, RADIUS_SMALL);
        if (isSelected) mCheckWhite.render(painter, checkRect);
    } else if (isSelected) {
        mCheckBlack.render(painter, checkRect);
    } else {
        painter->fillRect(option.rect, option.backgroundBrush);
    }

    int textLeft = baseRect.left() + 18;
    QRect textRect(textLeft, baseRect.top(), baseRect.width() - (textLeft - baseRect.left()), baseRect.height());
    painter->setPen(isHovered ? Qt::white : QColor(COLOR_TEXT_PRIMARY));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data().toString());
    painter->restore();
}

QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(22);
    return size;
}

// =====================================================
// FloatingScrollBarStyle 实现
// =====================================================

void FloatingScrollBarStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (control == CC_ScrollBar) {
        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if (!slider) {
            QProxyStyle::drawComplexControl(control, option, painter, widget);
            return;
        }

        QScrollBar *scrollbar = qobject_cast<QScrollBar*>(const_cast<QWidget*>(widget));
        if (!scrollbar) return;

        int range = scrollbar->maximum() - scrollbar->minimum();
        if (range < 0) return;

        const int padding = 2;

        int trackLength = slider->rect.height() - 2 * padding;
        int handleLength = qMax(trackLength * slider->pageStep / (range + slider->pageStep), 20);

        int handlePos = (trackLength - handleLength) * (scrollbar->value() - scrollbar->minimum()) / (range > 0 ? range : 1);

        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::white);
        painter->drawRect(slider->rect);

        QRect handleRect(slider->rect.right() - 8, slider->rect.top() + padding + handlePos, 6, handleLength);
        QColor color(102, 102, 102);
        color.setAlpha(slider->state & State_MouseOver ? 255 : 204);

        painter->save();
        painter->setBrush(color);
        painter->drawRoundedRect(handleRect, 3, 3);
        painter->restore();

        return;
    }

    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

// =====================================================
// MacStyleComboBox 实现
// =====================================================

MacStyleComboBox::MacStyleComboBox(QWidget *parent)
    : MacStyleComboBox(QString(), parent)
{
}

MacStyleComboBox::MacStyleComboBox(const QString &text, QWidget *parent)
    : QWidget(parent)
    , mText(text)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(23);

    addDropShadowEffect(this);

    mPopup = new PopupWidget(this);
    mModel = new QStringListModel(this);
    mPopup->listView()->setModel(mModel);
    mPopup->listView()->setItemDelegate(new ComboBoxDelegate(mPopup->listView()));
    mPopup->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    mPopup->listView()->setFrameShape(QFrame::NoFrame);
    mPopup->listView()->setLineWidth(0);
    mPopup->listView()->setMidLineWidth(0);
    mPopup->listView()->verticalScrollBar()->setStyle(new FloatingScrollBarStyle());

    connect(mPopup->listView(), &QListView::clicked, this, &MacStyleComboBox::onListClicked);

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MacStyleComboBox::onThemeChanged);
}

void MacStyleComboBox::onThemeChanged()
{
    update();
}

void MacStyleComboBox::addItem(const QString &text)
{
    int r = mModel->rowCount();
    mModel->insertRow(r);
    mModel->setData(mModel->index(r), text);
    if (mCurrentIndex < 0) setCurrentIndex(0);
}

void MacStyleComboBox::addItems(const QStringList &list)
{
    for (const QString &s : list) addItem(s);
}

void MacStyleComboBox::clear()
{
    mModel->removeRows(0, mModel->rowCount());
    mCurrentIndex = -1;
    update();
}

void MacStyleComboBox::setCurrentIndex(int idx)
{
    if (idx < 0 || idx >= mModel->rowCount()) return;
    if (mCurrentIndex == idx) return;
    mCurrentIndex = idx;
    update();
    emit currentIndexChanged(mCurrentIndex);
    emit currentTextChanged(currentText());
}

void MacStyleComboBox::setCurrentText(const QString &text)
{
    int index = -1;
    for (int i = 0; i < mModel->rowCount(); ++i) {
        QModelIndex idx = mModel->index(i, 0);
        if (mModel->data(idx, Qt::DisplayRole).toString() == text) {
            index = i;
            break;
        }
    }
    setCurrentIndex(index);
}

void MacStyleComboBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    int radius = RADIUS_SMALL;

    painter.setBrush(QColor(COLOR_WHITE));
    painter.setPen(QColor(COLOR_BORDER_DARK));
    painter.drawRoundedRect(rect, radius, radius);

    painter.setPen(QColor(COLOR_TEXT_PRIMARY));
    QRect textRect = rect.adjusted(8, 0, -22, 0);
    QFontMetrics fm = painter.fontMetrics();
    QString elided = fm.elidedText(currentText(), Qt::ElideRight, textRect.width());
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

    int arrowSize = rect.height() * 0.85;
    int arrowX = rect.width() - arrowSize - 1;
    int arrowY = (rect.height() - arrowSize) / 2 + 1;
    QRect arrowRect(arrowX, arrowY, arrowSize, arrowSize);

    QColor arrowBackgroundColor = mPressed ? ThemeManager::instance().accentPressedColor() : ThemeManager::instance().primaryColor();
    painter.setBrush(arrowBackgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(arrowRect, radius, radius);

    QSvgRenderer renderer(QString(":/ico/arrow_white.svg"));
    painter.setPen(Qt::NoPen);
    renderer.render(&painter, arrowRect);
}

void MacStyleComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mPressed = true;
        update();
    }
    QWidget::mousePressEvent(event);
}

void MacStyleComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (mPressed)
    {
        mPressed = false;
        update();
        if (rect().contains(event->pos())) {
            togglePopup();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void MacStyleComboBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down) {
        if (!mPopup->isVisible()) showPopup();
        else navigate(1);
        return;
    }
    if (event->key() == Qt::Key_Up) {
        if (mPopup->isVisible()) navigate(-1);
        return;
    }
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && mPopup->isVisible()) {
        QModelIndex idx = mPopup->listView()->currentIndex();
        if (idx.isValid()) {
            setCurrentIndex(idx.row());
            hidePopup();
            emit activated(mCurrentIndex);
        }
        return;
    }

    QWidget::keyPressEvent(event);
}

QSize MacStyleComboBox::sizeHint() const
{
    QFontMetrics fm(font());

    int maxTextWidth = 40;
    for (int i = 0; i < mModel->rowCount(); ++i) {
        QString text = mModel->data(mModel->index(i, 0), Qt::DisplayRole).toString();
        int w = fm.horizontalAdvance(text);
        if (w > maxTextWidth)
            maxTextWidth = w;
    }

    const int padding = 38;
    int width = maxTextWidth + padding;
    width = qMin(width, 1000);

    int height = fm.height();

    return QSize(width, height);
}

QSize MacStyleComboBox::minimumSizeHint() const
{
    QSize originalSize = sizeHint();
    int width = originalSize.width();
    if (width > 200)  width = 200;
    int height = originalSize.height();
    return QSize(width, height);
}

void MacStyleComboBox::onListClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    setCurrentIndex(index.row());
    hidePopup();
    emit activated(mCurrentIndex);
}

void MacStyleComboBox::showPopup()
{
    if (mModel->rowCount() == 0)
        return;

    auto *listView = mPopup->listView();
    QFontMetrics fm(listView->font());

    int maxTextWidth = 0;
    for (int row = 0; row < mModel->rowCount(); ++row) {
        QString text = mModel->data(mModel->index(row, 0), Qt::DisplayRole).toString();
        maxTextWidth = qMax(maxTextWidth, fm.horizontalAdvance(text));
    }

    int padding = 50;
    int popupWidth = maxTextWidth + padding;

    int rowHeight = listView->sizeHintForRow(0);
    if (rowHeight <= 0) rowHeight = fm.height() + 8;

    const int maxVisibleRows = 9;
    int visibleRows = qMin(maxVisibleRows, mModel->rowCount());
    int popupHeight = visibleRows * rowHeight + 12;

    if (mModel->rowCount() > maxVisibleRows)
        popupWidth += 15;

    listView->setMinimumWidth(popupWidth - 12);
    mPopup->resize(popupWidth, popupHeight);

    QPoint globalPos = this->mapToGlobal(QPoint(0, this->height()));

    if (mCurrentIndex >= 0) {
        QModelIndex curIndex = mModel->index(mCurrentIndex);
        listView->setCurrentIndex(curIndex);
        listView->scrollTo(curIndex, QAbstractItemView::PositionAtCenter);

        QRect itemRect = listView->visualRect(curIndex);
        globalPos -= QPoint(itemRect.left() + 20, itemRect.top() + rowHeight + 8);
    }

    mPopup->move(globalPos);
    mPopup->show();
    mPopup->raise();
    listView->setFocus();
}

void MacStyleComboBox::hidePopup()
{
    mPopup->hide();
}

void MacStyleComboBox::togglePopup()
{
    if (mPopup->isVisible()) hidePopup();
    else showPopup();
}

int MacStyleComboBox::calcPopupWidth() const
{
    QFontMetrics fm(font());
    int maxw = 0;
    for (int i = 0; i < mModel->rowCount(); ++i) {
        QString t = mModel->data(mModel->index(i)).toString();
        int w = fm.horizontalAdvance(t);
        if (w > maxw) maxw = w;
    }
    return maxw + 45;
}

void MacStyleComboBox::navigate(int step)
{
    int n = mModel->rowCount();
    if (n == 0) return;
    int idx = mPopup->listView()->currentIndex().row();
    if (idx < 0) idx = (mCurrentIndex >= 0) ? mCurrentIndex : 0;
    idx = qBound(0, idx + step, n - 1);
    QModelIndex newIdx = mModel->index(idx);
    mPopup->listView()->setCurrentIndex(newIdx);
    mPopup->listView()->scrollTo(newIdx, QAbstractItemView::PositionAtCenter);
}
