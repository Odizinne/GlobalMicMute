#ifndef GMM_H
#define GMM_H

#include <QWidget>
#include <QSystemTrayIcon>
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
    OverlayWidget *overlayWidget;
    bool isMuted;
    static const int HOTKEY_ID = 1;
    void createTrayIcon();
    void onConfiguratorClosed();
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    void toggleMicMute();
    void toggleMutedOverlay(bool enabled);
};
#endif // GMM_H
