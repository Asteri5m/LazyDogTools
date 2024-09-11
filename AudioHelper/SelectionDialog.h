#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <QDialog>
#include "CustomWidget.h"
#include "AudioCustom.h"
#include "TaskMonitor.h"

class SelectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectionDialog(QWidget *parent = nullptr);

    QString getSelectedOption() const;

signals:

private slots:
    void updateSelection(const QString &text);
    void onWindowItemClicked(const QModelIndex &index);
    void onProcessItemClicked(const QModelIndex &index);

private:
    QString mSelectedOption;
    TaskMonitor* mTaskMonitor;
};

#endif // SELECTIONDIALOG_H
