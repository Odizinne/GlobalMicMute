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
}

#endif // UTILS_H
