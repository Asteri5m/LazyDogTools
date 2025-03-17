/**
 * @file AudioManager.cpp
 * @author Asteri5m
 * @date 2025-02-08 0:33:44
 * @brief 音频管理器
 */

//音频相关
#include <Mmdeviceapi.h>
#include <Endpointvolume.h>
#include <functiondiscoverykeys.h>
#include <Audioclient.h>
#include <Devicetopology.h>
#include <QDebug>
#include "PolicyConfig.h"
#include "AudioManager.h"


AudioManager::AudioManager()
{
    mCurrentOutDeviceId = getDefaultAudioOutDevice();
}

AudioDeviceList AudioManager::getAudioOutDeviceList()
{
    AudioDeviceList audioOutDeviceDist;
    qDebug("Audio Output Devices:");
    // 初始化COM接口
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        qDebug("Failed to initialize COM.");
        return audioOutDeviceDist;
    }

    IMMDeviceEnumerator* pEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) 
    {
        qDebug("Failed to create device enumerator.");
        CoUninitialize();
        return audioOutDeviceDist;
    }

    // 获取设备集合
    IMMDeviceCollection* pDeviceCollection = NULL;
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDeviceCollection);
    if (FAILED(hr)) 
    {
        qDebug("Failed to enumerate audio endpoints.");
        return audioOutDeviceDist;
    }

    UINT deviceCount;
    pDeviceCollection->GetCount(&deviceCount);
    // 遍历设备
    for (UINT i = 0; i < deviceCount; i++) 
    {
        IMMDevice* pDevice = NULL;
        pDeviceCollection->Item(i, &pDevice);
        // 获取设备ID
        LPWSTR pwszID = NULL;
        pDevice->GetId(&pwszID);

        IPropertyStore* pProps = NULL;
        hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
        if (SUCCEEDED(hr)) 
        {
            // 获取设备友好名字
            PROPVARIANT varName;
            PropVariantInit(&varName);
            hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
            if (SUCCEEDED(hr)) 
            {
                QString deviceName = QString::fromWCharArray(varName.pwszVal);
                QString deviceID = QString::fromWCharArray(pwszID);
                qDebug() << "Device" << i + 1 << ":" << deviceID << "|" << deviceName;
                PropVariantClear(&varName);
                audioOutDeviceDist.insert(deviceName, deviceID);
            }

            pProps->Release();
        }
        CoTaskMemFree(pwszID);
        pDevice->Release();
    }
    pDeviceCollection->Release();
    return audioOutDeviceDist;
}

QString AudioManager::getDefaultAudioOutDevice()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        qDebug("Failed to initialize COM.");
        return "0";
    }

    IMMDeviceEnumerator* pEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        qDebug("Failed to create device enumerator.");
        CoUninitialize();
        return "0";
    }

    // 获取设备
    IMMDevice* pDefaultPlaybackDevice = NULL;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultPlaybackDevice);
    if (SUCCEEDED(hr))
    {
        // 获取设备ID
        LPWSTR pwszID = NULL;
        pDefaultPlaybackDevice->GetId(&pwszID);
        QString deviceID = QString::fromWCharArray(pwszID);
        return deviceID;
    }
    return "0";
}

bool AudioManager::setAudioOutDevice(const QString &deviceId)
{
    // 将QString转换为UTF-16编码的wchar_t数组
    const wchar_t* wcharStr = reinterpret_cast<const wchar_t*>(deviceId.utf16());
    // 将wchar_t数组转换为LPWSTR
    LPWSTR devID = const_cast<LPWSTR>(wcharStr);
    IPolicyConfigVista* pPolicyConfig = nullptr;
    ERole reserved = eConsole;
    char* errorMsg = nullptr;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
                                  NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
    if (SUCCEEDED(hr)) {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
        pPolicyConfig->Release();
    }

    // 成功返回true
    if (SUCCEEDED(hr)) {
        mCurrentOutDeviceId = deviceId;
        return true;
    }

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorMsg, 0, NULL);
    qCritical() << "SetDefaultEndpoint failed - Error code:" << QString::number(hr, 16).toUpper().toUtf8().constData()
                << ". Description:" << QString::fromLocal8Bit(errorMsg).trimmed().toUtf8().constData();
    return false;
}

QString AudioManager::getCurrentAudioOutDevice()
{
    return mCurrentOutDeviceId;
}
