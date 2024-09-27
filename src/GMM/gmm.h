#ifndef GMM_H
#define GMM_H

#include <QWidget>
#include <QSystemTrayIcon>

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
    bool isMuted;
    static const int HOTKEY_ID = 1;
    void createTrayIcon();
    void onConfiguratorClosed();
    bool registerGlobalHotkey();
    void unregisterGlobalHotkey();
    void toggleMicMute();
};
#endif // GMM_H
