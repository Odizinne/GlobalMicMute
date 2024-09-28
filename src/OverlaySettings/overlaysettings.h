#ifndef OVERLAYSETTINGS_H
#define OVERLAYSETTINGS_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace Ui {
class OverlaySettings;
}

class OverlaySettings : public QMainWindow
{
    Q_OBJECT

public:
    explicit OverlaySettings(QWidget *parent = nullptr);
    ~OverlaySettings();

private slots:
    void onDisableOverlayStateChanged();
    void onStartupCheckBoxStateChanged();

private:
    Ui::OverlaySettings *ui;
    QJsonObject settings;
    static const QString settingsFile;
    void loadSettings();
    void applySettings();
    void saveSettings();

signals:
    void settingsChanged();
    void closed();
};

#endif // OVERLAYSETTINGS_H
