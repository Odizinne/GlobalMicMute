#ifndef GMM_H
#define GMM_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "overlaywidget.h"
#include "overlaysettings.h"

class GMM : public QWidget
{
    Q_OBJECT

public:
    GMM(QWidget *parent = nullptr);
    ~GMM();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon *trayIcon;
    QAction *settingsAction;
    QAction *exitAction;
    bool disableNotification;
    bool disableOverlay;
    OverlayWidget *overlayWidget;
    OverlaySettings *overlaySettings;
    QJsonObject settings;
    QString position;
    static const QString settingsFile;
    bool isMuted;
    static const int HOTKEY_ID = 1;
    void createTrayIcon();
    void onConfiguratorClosed();
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    void toggleMicMute();
    void toggleMutedOverlay(bool enabled);
    void sendNotification(bool enabled);
    void loadSettings();
    void applySettings();
    void onSettingsChanged();
    void onSettingsClosed();
    void showSettings();
};
#endif // GMM_H
