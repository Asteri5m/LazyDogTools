#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

/**
 * @file AudioManager.h
 * @author Asteri5m
 * @date 2025-02-08 0:33:44
 * @brief 音频管理器
 */

#include <QMap>

//音频设备---key:friendname,value:deviceId
typedef QMap<QString, QString> AudioDeviceList;

class AudioManager
{
public:
    AudioManager();
    static AudioDeviceList getAudioOutDeviceList();
    static QString getDefaultAudioOutDevice();
    bool setAudioOutDevice(const QString &deviceId);
    QString getCurrentAudioOutDevice();

private:
    QString mCurrentOutDeviceId;
};

#endif // AUDIOMANAGER_H
