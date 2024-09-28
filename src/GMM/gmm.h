#ifndef GMM_H
#define GMM_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "overlaywidget.h"

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
    void onStartupMenuEntryChecked(bool checked);

private:
    QSystemTrayIcon *trayIcon;
    QAction *startupAction;
    QAction *notificationAction;
    QAction *overlayAction;
    QAction *exitAction;
    OverlayWidget *overlayWidget;
    QJsonObject settings;
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
    void saveSettings();
};
#endif // GMM_H
