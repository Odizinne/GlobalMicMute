#include "utils.h"
#include <windows.h>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <windows.h>

QString Utils::getTheme()
{
    // Determine the theme based on registry value
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat);
    int value = settings.value("AppsUseLightTheme", 1).toInt();

    return (value == 0) ? "light" : "dark";
}

void Utils::playSoundNotification(bool enabled)
{
    const wchar_t* soundFile;

    if (enabled) {
        soundFile = L"C:\\Windows\\Media\\Speech On.wav";
    } else {
        soundFile = L"C:\\Windows\\Media\\Speech Off.wav";
    }

    PlaySound(soundFile, NULL, SND_FILENAME | SND_ASYNC);
}

int getBuildNumber()
{
    HKEY hKey;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                 TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
                 0, KEY_READ, &hKey);

    char buildNumberString[256];
    DWORD bufferSize = sizeof(buildNumberString);
    RegQueryValueEx(hKey, TEXT("CurrentBuild"), NULL, NULL, (LPBYTE)buildNumberString, &bufferSize);
    RegCloseKey(hKey);

    return std::stoi(buildNumberString);
}

bool Utils::isWindows10()
{
    int buildNumber = getBuildNumber();
    return (buildNumber >= 10240 && buildNumber < 22000);
}

bool Utils::SetMicrophoneMute(bool mute)
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return false;

    // Get the device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL,
        CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator);

    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    // Get the default audio capture device (microphone)
    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pDevice);
    if (FAILED(hr))
    {
        pEnumerator->Release();
        CoUninitialize();
        return false;
    }

    // Get the audio endpoint volume control interface
    IAudioEndpointVolume* pAudioEndpointVolume = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pAudioEndpointVolume);
    if (FAILED(hr))
    {
        pDevice->Release();
        pEnumerator->Release();
        CoUninitialize();
        return false;
    }

    // Set the mute state
    hr = pAudioEndpointVolume->SetMute(mute, NULL);

    // Cleanup
    pAudioEndpointVolume->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();

    return SUCCEEDED(hr);
}

bool Utils::GetMicrophoneMuteStatus()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return false; // Return false in case of failure

    // Get the device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL,
        CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator);

    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    // Get the default audio capture device (microphone)
    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pDevice);
    if (FAILED(hr))
    {
        pEnumerator->Release();
        CoUninitialize();
        return false;
    }

    // Get the audio endpoint volume control interface
    IAudioEndpointVolume* pAudioEndpointVolume = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pAudioEndpointVolume);
    if (FAILED(hr))
    {
        pDevice->Release();
        pEnumerator->Release();
        CoUninitialize();
        return false;
    }

    // Get the mute state
    BOOL mute = FALSE;
    hr = pAudioEndpointVolume->GetMute(&mute);

    // Cleanup
    pAudioEndpointVolume->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();

    // Return true if microphone is muted, false if not
    return mute == TRUE;
}

QIcon Utils::getIcon(bool muted)
{
    QString theme = getTheme();
    ;
    if (muted)
        return QIcon(QString(":/icons/tray_icon_%1_muted.png").arg(theme));
    return QIcon(QString(":/icons/tray_icon_%1.png").arg(theme));
}
