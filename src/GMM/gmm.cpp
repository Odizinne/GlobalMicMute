#include "gmm.h"
#include <QApplication>
#include "utils.h"
#include "shortcutmanager.h"
#include <QMenu>
#include <iostream>
#include <windows.h>

using namespace Utils;
using namespace ShortcutManager;

GMM::GMM(QWidget *parent)
    : QWidget(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , isMuted(GetMicrophoneMuteStatus())
{
    createTrayIcon();
    if (!registerGlobalHotkey()) {
        qWarning() << "Failed to register global hotkey";
    }
}

GMM::~GMM()
{
    unregisterGlobalHotkey();
}

void GMM::createTrayIcon()
{
    trayIcon->setIcon(getIcon(isMuted));
    QMenu *trayMenu = new QMenu(this);

    QAction *startupAction = new QAction("Run at startup", this);
    startupAction->setCheckable(true);
    startupAction->setChecked(isShortcutPresent());
    connect(startupAction, &QAction::triggered, this, &GMM::onStartupMenuEntryChecked);
    trayMenu->addAction(startupAction);

    QAction *exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setToolTip("Global mic mute");
    trayIcon->show();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &GMM::trayIconActivated);
}

void GMM::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        toggleMicMute();
    }
}

bool GMM::registerGlobalHotkey() {
    return RegisterHotKey((HWND)this->winId(), HOTKEY_ID, MOD_CONTROL | MOD_ALT, 0x4D);
}

void GMM::unregisterGlobalHotkey() {
    UnregisterHotKey((HWND)this->winId(), HOTKEY_ID);
}

bool GMM::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == HOTKEY_ID) {
            toggleMicMute();
            return true;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

void GMM::toggleMicMute()
{
    isMuted = !isMuted;
    bool success = SetMicrophoneMute(isMuted);
    if (success) {
        std::cout << (isMuted ? "Microphone Muted" : "Microphone Unmuted") << std::endl;
        trayIcon->setIcon(getIcon(isMuted));
        if (GetMicrophoneMuteStatus()) {
            playSoundNotification(false);
        } else {
            playSoundNotification(true);
        }
    }

}

void GMM::onStartupMenuEntryChecked(bool checked)
{
    manageShortcut(checked);
}
