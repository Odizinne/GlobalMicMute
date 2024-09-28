#include "gmm.h"
#include "utils.h"
#include "shortcutmanager.h"
#include <windows.h>
#include <QApplication>
#include <QMenu>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

using namespace Utils;
using namespace ShortcutManager;

const QString GMM::settingsFile = QStandardPaths::writableLocation(
                                               QStandardPaths::AppDataLocation)
                                           + "/GlobalMicMute/settings.json";

GMM::GMM(QWidget *parent)
    : QWidget(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , overlayWidget(nullptr)
    , isMuted(GetMicrophoneMuteStatus())
{
    loadSettings();
    createTrayIcon();
    registerGlobalHotkey();
    toggleMutedOverlay(isMuted);
}

GMM::~GMM()
{
    unregisterGlobalHotkey();
    delete overlayWidget;
}

void GMM::createTrayIcon()
{
    trayIcon->setIcon(getIcon(isMuted));
    QMenu *trayMenu = new QMenu(this);

    startupAction = new QAction("Run at startup", this);
    startupAction->setCheckable(true);
    startupAction->setChecked(isShortcutPresent());
    connect(startupAction, &QAction::triggered, this, &GMM::onStartupMenuEntryChecked);
    trayMenu->addAction(startupAction);

    overlayAction = new QAction("Disable overlay", this);
    overlayAction->setCheckable(true);
    overlayAction->setChecked(settings.value("overlay").toBool());
    connect(overlayAction, &QAction::triggered, this, &GMM::saveSettings);
    trayMenu->addAction(overlayAction);

    notificationAction = new QAction("Disable notification", this);
    notificationAction->setCheckable(true);
    notificationAction->setChecked(settings.value("notification").toBool());
    connect(notificationAction, &QAction::triggered, this, &GMM::saveSettings);
    trayMenu->addAction(notificationAction);

    exitAction = new QAction("Exit", this);
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
        trayIcon->setIcon(getIcon(isMuted));
        if (GetMicrophoneMuteStatus()) {
            sendNotification(true);
            toggleMutedOverlay(true);
        } else {
            sendNotification(false);
            toggleMutedOverlay(false);
        }
    }
}

void GMM::onStartupMenuEntryChecked(bool checked)
{
    manageShortcut(checked);
}

void GMM::toggleMutedOverlay(bool enabled)
{
    if (overlayAction->isChecked()) {
        return;
    }

    if (enabled) {
        if (overlayWidget == nullptr) {
            overlayWidget = new OverlayWidget(this);
        }
        overlayWidget->show();
    } else {
        if (overlayWidget != nullptr) {
            delete overlayWidget;
            overlayWidget = nullptr;
        }
    }
}

void GMM::sendNotification(bool enabled)
{
    if (notificationAction->isChecked()) {
        return;
    }

    if (enabled) {
        playSoundNotification(false);
    } else {
        playSoundNotification(true);
    }
}

void GMM::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        return;

    } else {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                settings = doc.object();
            }
            file.close();
        }
    }
}

void GMM::saveSettings()
{
    settings["overlay"] = overlayAction->isChecked();
    settings["notification"] = notificationAction->isChecked();

    QFile file(settingsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}
