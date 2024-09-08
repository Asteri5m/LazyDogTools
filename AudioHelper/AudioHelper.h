#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#include "CustomWidget.h"
#include <QListWidget>

class AudioHelper : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit AudioHelper(QWidget *parent = nullptr);

signals:

private slots:
    void onItemClicked(QListWidgetItem *item);
    void buttonClicked();
    void checkBoxChecked(bool);
    void comboBoxChanged(QString);

private:
    QWidget *mHomePage;
    QWidget *mPrefsPage;
    QListWidget *mTaskTab;

    MacStyleButton *mGameButton;

    void initHomePage();
    void initPrefsPage();
};

#endif // AUDIOHELPER_H
