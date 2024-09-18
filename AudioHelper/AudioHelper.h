#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#include "CustomWidget.h"
#include "AudioCustom.h"
#include <QListWidget>

typedef QList<RelatedItem> RelatedList;

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
    RelatedList *mRelatedList;

    MacStyleButton *mGameButton;

    void initHomePage();
    void initPrefsPage();
    void addRelatedItem();
    void delRelatedItem();
    void changeRelatedItem();
    void setGameTag(bool);
};

#endif // AUDIOHELPER_H
