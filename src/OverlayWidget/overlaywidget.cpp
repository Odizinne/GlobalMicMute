#include "OverlayWidget.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QtMath>

OverlayWidget::OverlayWidget(QWidget *parent) : QWidget(parent), opacityFactor(0), increasing(true)
{
    setFixedSize(80, 80);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    const QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = screenGeometry.width() - width() - 20;
    int y = 20;
    move(x, y);

    setWindowOpacity(opacityFactor);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &OverlayWidget::updateOpacity);
    timer->start(50);
}

void OverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width(), height());
    QPixmap icon(":/icons/overlay_dark.png");

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
    double speed = 0.04;
    if (increasing)
    {
        opacityFactor += speed;
        if (opacityFactor >= 1.0)
            increasing = false;
    }
    else
    {
        opacityFactor -= speed;
        if (opacityFactor <= 0.0)
            increasing = true;
    }

    setWindowOpacity(opacityFactor);
    update();
}
