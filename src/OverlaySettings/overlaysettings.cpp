#include "overlaysettings.h"
#include "ui_overlaysettings.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <shortcutmanager.h>

using namespace ShortcutManager;
const QString OverlaySettings::settingsFile = QStandardPaths::writableLocation(
                                                  QStandardPaths::AppDataLocation)
                                              + "/GlobalMicMute/settings.json";

OverlaySettings::OverlaySettings(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OverlaySettings)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    ui->startupCheckBox->setChecked(isShortcutPresent());
    loadSettings();
    applySettings();

    QList<QRadioButton*> radioButtons = {
        ui->topLeftCorner,
        ui->topRightCorner,
        ui->topCenter,
        ui->bottomLeftCorner,
        ui->bottomRightCorner,
        ui->bottomCenter,
        ui->leftCenter,
        ui->rightCenter
    };

    for (QRadioButton* button : radioButtons) {
        connect(button, &QRadioButton::clicked, this, &OverlaySettings::saveSettings);
    }

    connect(ui->overlayCheckBox, &QCheckBox::stateChanged, this, &OverlaySettings::onDisableOverlayStateChanged);
    connect(ui->soundCheckBox, &QCheckBox::stateChanged, this, &OverlaySettings::saveSettings);
    connect(ui->potatoModeCheckBox, &QCheckBox::stateChanged, this, &OverlaySettings::saveSettings);
    connect(ui->startupCheckBox, &QCheckBox::stateChanged, this, &OverlaySettings::onStartupCheckBoxStateChanged);
}

OverlaySettings::~OverlaySettings()
{
    emit closed();
    delete ui;
}

void OverlaySettings::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        saveSettings();
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

void OverlaySettings::applySettings()
{
    QString position = settings.value("overlayPosition").toString();

    QList<QPair<QRadioButton*, QString>> radioButtons = {
        { ui->bottomCenter, "bottomCenter" },
        { ui->bottomLeftCorner, "bottomLeftCorner" },
        { ui->bottomRightCorner, "bottomRightCorner" },
        { ui->topCenter, "topCenter" },
        { ui->topRightCorner, "topRightCorner" },
        { ui->topLeftCorner, "topLeftCorner" },
        { ui->leftCenter, "leftCenter" },
        { ui->rightCenter, "rightCenter" }
    };

    for (const auto& pair : radioButtons) {
        if (position == pair.second) {
            pair.first->setChecked(true);
            break;
        }
    }

    ui->overlayCheckBox->setChecked(settings.value("disableOverlay").toBool());
    ui->soundCheckBox->setChecked(settings.value("disableNotification").toBool());
    ui->potatoModeCheckBox->setChecked(settings.value("potatoMode").toBool());
}

void OverlaySettings::saveSettings()
{
    qDebug() << "pass";
    QString position;

    QList<QPair<QRadioButton*, QString>> radioButtons = {
        { ui->bottomCenter, "bottomCenter" },
        { ui->bottomLeftCorner, "bottomLeftCorner" },
        { ui->bottomRightCorner, "bottomRightCorner" },
        { ui->topCenter, "topCenter" },
        { ui->topRightCorner, "topRightCorner" },
        { ui->topLeftCorner, "topLeftCorner" },
        { ui->leftCenter, "leftCenter" },
        { ui->rightCenter, "rightCenter" }
    };

    for (const auto& pair : radioButtons) {
        if (pair.first->isChecked()) {
            position = pair.second;
            break;
        }
    }

    settings["overlayPosition"] = position;
    settings["disableOverlay"] = ui->overlayCheckBox->isChecked();
    settings["disableNotification"] = ui->soundCheckBox->isChecked();
    settings["potatoMode"] = ui->potatoModeCheckBox->isChecked();

    QFile file(settingsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
    emit settingsChanged();
}

void OverlaySettings::onDisableOverlayStateChanged()
{
    ui->frame->setDisabled(ui->overlayCheckBox->isChecked());
    ui->label->setDisabled(ui->overlayCheckBox->isChecked());
    saveSettings();
}

void OverlaySettings::onStartupCheckBoxStateChanged()
{
    manageShortcut(ui->startupCheckBox->isChecked());
}
