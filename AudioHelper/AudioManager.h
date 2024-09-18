#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

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
