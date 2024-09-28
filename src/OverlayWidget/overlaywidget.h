#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QTimer>

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    OverlayWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateOpacity();

private:
    QTimer *timer;
    double opacityFactor;
    bool increasing;
};

#endif // OVERLAYWIDGET_H
