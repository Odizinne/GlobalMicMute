#include "OverlayWidget.h"
#include "utils.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QTimer>
#include <QtMath>
#include <QPainterPath>

using namespace Utils;

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget(parent)
    , opacityFactor(0.3)
    , increasing(true)
{
    setFixedSize(80, 80);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QPainterPath path;
    setMask(QRegion(path.toFillPolygon().toPolygon()));

    const QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = screenGeometry.width() - width() - 25;
    int y = 25;
    move(x, y);

    setWindowOpacity(opacityFactor);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &OverlayWidget::updateOpacity);
    timer->start(16);
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
