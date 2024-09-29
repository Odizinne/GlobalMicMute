#include "OverlayWidget.h"
#include "utils.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QTimer>
#include <QtMath>
#include <QPainterPath>
#include <QMap>

using namespace Utils;

OverlayWidget::OverlayWidget(QString& position, bool& potatoMode, QWidget *parent)
    : QWidget(parent)
    , opacityFactor(0.3)
    , increasing(true)
{
    setFixedSize(80, 80);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QPainterPath path;
    setMask(QRegion(path.toFillPolygon().toPolygon()));

    moveOverlayToPosition(position);
    setWindowOpacity(opacityFactor);

    if (!potatoMode) {
        opacityFactor = 0.3;
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &OverlayWidget::updateOpacity);
        timer->start(16);
    } else {
        opacityFactor = 1;
        setWindowOpacity(opacityFactor);
    }
}

void OverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width(), height());

    QString theme = getTheme();
    QString iconTheme = (theme == "light") ? "dark" : "light";
    QString iconPath = QString(":/icons/overlay_%1.png").arg(iconTheme);
    QPixmap icon(iconPath);

    if (!icon.isNull())
    {
        int iconWidth = icon.width();
        int iconHeight = icon.height();
        int x = (width() - iconWidth) / 2;
        int y = (height() - iconHeight) / 2;

        painter.drawPixmap(x, y, icon);
    }
}

void OverlayWidget::updateOpacity()
{
    double speed = 0.0075;
    double minOpacity = 0.3;
    double maxOpacity = 1.0;

    if (increasing)
    {
        opacityFactor += speed;
        if (opacityFactor >= maxOpacity)
            increasing = false;
    }
    else
    {
        opacityFactor -= speed;
        if (opacityFactor <= minOpacity)
            increasing = true;
    }

    setWindowOpacity(opacityFactor);
    update();
}

void OverlayWidget::moveOverlayToPosition(const QString &position)
{
    const QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int margin = 25;
    int taskbarOffset = 46;

    QMap<QString, std::function<QPoint()>> positionMap = {
                                                          {"topLeftCorner",      [=]() { return QPoint(margin, margin); }},
                                                          {"topRightCorner",     [=]() { return QPoint(screenWidth - width() - margin, margin); }},
                                                          {"topCenter",          [=]() { return QPoint((screenWidth - width()) / 2, margin); }},
                                                          {"bottomLeftCorner",   [=]() { return QPoint(margin, screenHeight - height() - margin - taskbarOffset); }},
                                                          {"bottomRightCorner",  [=]() { return QPoint(screenWidth - width() - margin, screenHeight - height() - margin - taskbarOffset); }},
                                                          {"bottomCenter",       [=]() { return QPoint((screenWidth - width()) / 2, screenHeight - height() - margin - taskbarOffset); }},
                                                          {"leftCenter",         [=]() { return QPoint(margin, (screenHeight - height()) / 2); }},
                                                          {"rightCenter",        [=]() { return QPoint(screenWidth - width() - margin, (screenHeight - height()) / 2); }},
                                                          };

    if (positionMap.contains(position)) {
        QPoint newPos = positionMap[position]();
        move(newPos);
    }
}
