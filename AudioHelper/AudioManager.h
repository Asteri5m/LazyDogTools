#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QMap>

//音频设备---key:friendname,value:deviceId
typedef QMap<QString, QString> AudioDeviceList;

class AudioManager
{
public:
    AudioManager();
    static AudioDeviceList GetAudioOutDeviceList();
    static QString GetDefaultAudioOutDevice();
    bool SetAudioOutDevice(const QString &deviceId);
    QString GetCurrentAudioOutDevice();


private:
    QString mCurrentOutDeviceId;
};

#endif // AUDIOMANAGER_H
