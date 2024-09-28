#include "gmm.h"
#include "utils.h"
#include <windows.h>
#include <QApplication>
#include <QMenu>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

using namespace Utils;

const QString GMM::settingsFile = QStandardPaths::writableLocation(
                                               QStandardPaths::AppDataLocation)
                                           + "/GlobalMicMute/settings.json";

GMM::GMM(QWidget *parent)
    : QWidget(parent)
    , trayIcon(new QSystemTrayIcon(this))
    , overlayWidget(nullptr)
    , overlaySettings(nullptr)
    , isMuted(GetMicrophoneMuteStatus())
{
    loadSettings();
    applySettings();
    createTrayIcon();
    registerGlobalHotkey();
    toggleMutedOverlay(isMuted);
}

GMM::~GMM()
{
    unregisterGlobalHotkey();
    delete overlayWidget;
    delete overlaySettings;
}

void GMM::createTrayIcon()
{
    trayIcon->setIcon(getIcon(isMuted));
    QMenu *trayMenu = new QMenu(this);

    settingsAction = new QAction("Settings", this);
    connect(settingsAction, &QAction::triggered, this, &GMM::showSettings);
    trayMenu->addAction(settingsAction);

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

void GMM::toggleMutedOverlay(bool enabled)
{
    if (disableOverlay) {
        if (overlayWidget != nullptr) {
            overlayWidget->hide();
            delete overlayWidget;
            overlayWidget = nullptr;
        }
        return;
    }

    if (enabled) {
        if (overlayWidget == nullptr) {
            overlayWidget = new OverlayWidget(position, this);
        }
        overlayWidget->show();
    } else {
        if (overlayWidget != nullptr) {
            overlayWidget->hide();
        }
    }
}

void GMM::sendNotification(bool enabled)
{
    if (disableNotification) {
        return;
    }

    if (enabled) {
        playSoundNotification(false);
    } else {
        playSoundNotification(true);
    }
}

void GMM::showSettings()
{
    if (overlaySettings) {
        overlaySettings->showNormal();
        overlaySettings->raise();
        overlaySettings->activateWindow();
        return;
    }

    overlaySettings = new OverlaySettings;
    overlaySettings->setAttribute(Qt::WA_DeleteOnClose);
    connect(overlaySettings, &OverlaySettings::closed, this, &GMM::onSettingsClosed);
    connect(overlaySettings, &OverlaySettings::settingsChanged, this, &GMM::onSettingsChanged);
    overlaySettings->show();
}

void GMM::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        disableNotification = false;
        disableOverlay = false;
        position = "topRightCorner";
        return;
    }

    if (file.open(QIODevice::ReadOnly)) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            settings = doc.object();
        }
        file.close();
    }
}

void GMM::applySettings()
{
    position = settings.value("overlayPosition").toString();
    disableOverlay = settings.value("disableOverlay").toBool();
    disableNotification = settings.value("disableNotification").toBool();
}

void GMM::onSettingsChanged()
{
    loadSettings();
    applySettings();

    if (isMuted) {
        toggleMutedOverlay(!disableOverlay);
    }

    if (!disableOverlay) {
        if (!overlayWidget) {
            overlayWidget = new OverlayWidget(position, this);
        }
        overlayWidget->moveOverlayToPosition(position);
    }
}

void GMM::onSettingsClosed()
{
    disconnect(overlaySettings, &OverlaySettings::closed, this, &GMM::onSettingsClosed);
    disconnect(overlaySettings, &OverlaySettings::settingsChanged, this, &GMM::onSettingsChanged);
    overlaySettings = nullptr;
}
