#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

/**
 * @file SelectionInfo.h
 * @author Asteri5m
 * @date 2025-02-08 0:46:13
 * @brief 任务关联项选择窗口
 */

#include <QDialog>
#include "CustomWidget.h"
#include "AudioCustom.h"
#include "TaskMonitor.h"

struct SelectionInfo {
    TaskInfo taskInfo;
    QString type;
};

class SelectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectionDialog(QWidget *parent = nullptr);

    SelectionInfo* selectedOption() const;

signals:

private slots:
    void updateSelection(const TaskInfo &taskInfo, const QString &type);
    void onWindowItemClicked(const QModelIndex &index);
    void onProcessItemClicked(const QModelIndex &index);

private:
    SelectionInfo* mSelectedOption;
    TaskMonitor* mTaskMonitor;
};

#endif // SELECTIONDIALOG_H
