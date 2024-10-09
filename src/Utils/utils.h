#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>
#include <QFrame>

namespace Utils {

    void playSoundNotification(bool enabled);
    void setFrameColorBasedOnWindow(QWidget *window, QFrame *frame);
    bool isDarkMode(const QColor &color);
    QColor adjustColor(const QColor &color, double factor);
    bool isWindows10();
    bool SetMicrophoneMute(bool mute);
    bool GetMicrophoneMuteStatus();
    QIcon getIcon(bool muted);
    QString getTheme();
    QString getAccentColor(const QString &accentKey);
    QPixmap createIconWithAccentBackground();
}

#endif // UTILS_H
