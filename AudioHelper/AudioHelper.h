#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#include "CustomWidget.h"
#include "AudioCustom.h"
#include "AudioHelperServer.h"
#include <QListWidget>


class AudioHelper : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit AudioHelper(QWidget *parent = nullptr);
    ~AudioHelper();
    QString queryConfig(const QString& key);

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
    QMap<QString, QString> mConfig;
    AudioHelperServer *mServer;

    MacStyleButton *mTagButton;

    void initHomePage();
    void initPrefsPage();
    void addRelatedItem();
    void delRelatedItem();
    void changeRelatedItem();
    void setSceneTag(bool isAdd);
    template <typename T>
    void loadConfigHandler(T* widget, const QString& defaultValue = QString());
};

#endif // AUDIOHELPER_H
