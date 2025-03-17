#ifndef AUDIOHELPERWIDGET_H
#define AUDIOHELPERWIDGET_H

/**
 * @file AudioHelperWidget.h
 * @author Asteri5m
 * @date 2025-02-08 0:07:54
 * @brief AudioHelper的交互窗口
 */

#include "CustomWidget.h"
#include "AudioCustom.h"
#include "Custom.h"
#include "AudioDatabase.h"


class AudioHelperWidget : public ToolWidgetModel {
    Q_OBJECT
public:
    explicit AudioHelperWidget(RelatedList *relatedList, QMap<QString, QString> *config, AudioDatabase* database, QWidget *parent = nullptr);
    ~AudioHelperWidget();
    QString queryConfig(const QString& key);

signals:
    void configChanged(const QString &key, const QString &value);

private slots:
    void onItemClicked(QTableWidgetItem *item);
    void buttonClicked();
    void checkBoxChecked(bool);
    void comboBoxChanged(QString);

private:
    QWidget *mHomePage;
    QWidget *mPrefsPage;
    TableWidget *mTaskTab;
    RelatedList *mRelatedList;
    QMap<QString, QString> *mConfig;
    AudioDatabase *mDatabase;
    MacStyleButton *mTagButton;

    void initHomePage();
    void initPrefsPage();
    void addRelatedItem();
    void delRelatedItem();
    void changeRelatedItem();
    void setSceneTag(bool isAdd);
    template <typename T>
    void loadConfigHandler(T* widget);
};


#endif // AUDIOHELPERWIDGET_H
