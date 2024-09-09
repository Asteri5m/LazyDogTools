#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <QDialog>
#include "CustomWidget.h"
#include "AudioCustom.h"

class SelectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectionDialog(QWidget *parent = nullptr);

    QString getSelectedOption() const;

signals:

private slots:
    void updateSelection(const QString &text);

private:
    QString mSelectedOption;
};

#endif // SELECTIONDIALOG_H
