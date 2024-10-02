#include "utils.h"
#include <QPainter>
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

// Helper function to convert a BYTE value to a hex string
QString toHex(BYTE value) {
    const char* hexDigits = "0123456789ABCDEF";
    return QString("%1%2")
        .arg(hexDigits[value >> 4])
        .arg(hexDigits[value & 0xF]);
}

// Function to fetch the accent color directly from the Windows registry
QString Utils::getAccentColor(const QString &accentKey)
{
    HKEY hKey;
    BYTE accentPalette[32];  // AccentPalette contains 32 bytes
    DWORD bufferSize = sizeof(accentPalette);

    // Open the Windows registry key for AccentPalette
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Read the AccentPalette binary data
        if (RegGetValueW(hKey, NULL, L"AccentPalette", RRF_RT_REG_BINARY, NULL, accentPalette, &bufferSize) == ERROR_SUCCESS) {
            // Close the registry key after reading
            RegCloseKey(hKey);

            // Determine the correct index based on the accentKey
            int index = 0;
            if (accentKey == "light3") index = 0;
            else if (accentKey == "light2") index = 4;
            else if (accentKey == "light1") index = 8;
            else if (accentKey == "normal") index = 12;
            else if (accentKey == "dark1") index = 16;
            else if (accentKey == "dark2") index = 20;
            else if (accentKey == "dark3") index = 24;
            else {
                qDebug() << "Invalid accentKey provided.";
                return "#FFFFFF";  // Return white if invalid accentKey
            }

            // Extract RGB values and convert them to hex format
            QString red = toHex(accentPalette[index]);
            QString green = toHex(accentPalette[index + 1]);
            QString blue = toHex(accentPalette[index + 2]);

            // Return the hex color code
            return QString("#%1%2%3").arg(red, green, blue);
        } else {
            qDebug() << "Failed to retrieve AccentPalette from the registry.";
        }

        RegCloseKey(hKey);  // Ensure the key is closed
    } else {
        qDebug() << "Failed to open registry key.";
    }

    // Fallback color if registry access fails
    return "#FFFFFF";
}

QPixmap Utils::createIconWithAccentBackground() {
    QString theme = Utils::getTheme();
    QString accentColorStr;
    if (theme == "light") {
        accentColorStr = Utils::getAccentColor("light2");
    } else {
        accentColorStr = Utils::getAccentColor("dark1");
    }

    QColor accentColor(accentColorStr);
    QPixmap background(80, 80);
    background.fill(accentColor);
    QPainter painter(&background);
    int brightness = (accentColor.red() * 0.299) + (accentColor.green() * 0.587) + (accentColor.blue() * 0.114);
    QString iconTheme = (brightness > 127) ? ":/icons/overlay_dark.png" : ":/icons/overlay_light.png";  // Bright background -> dark icon, dark background -> light icon

    QPixmap icon(iconTheme);

    int x = (background.width() - icon.width()) / 2;
    int y = (background.height() - icon.height()) / 2;

    painter.drawPixmap(x, y, icon);
    painter.end();

    return background;
}
