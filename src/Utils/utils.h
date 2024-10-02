#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>

namespace Utils {

    void playSoundNotification(bool enabled);
    bool isWindows10();
    bool SetMicrophoneMute(bool mute);
    bool GetMicrophoneMuteStatus();
    QIcon getIcon(bool muted);
    QString getTheme();
    QString getAccentColor(const QString &accentKey);
    QPixmap createIconWithAccentBackground();
}

#endif // UTILS_H
