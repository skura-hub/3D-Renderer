#ifndef MYPAINTIMAGE_H
#define MYPAINTIMAGE_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QSize>
#include <QResizeEvent>
#include <QDebug>
#include <QVector3D>
#include <QVector2D>
#include <QWheelEvent>
#include <QMouseEvent>


class MyPaintImage : public QWidget
{
    Q_OBJECT
private:
    QImage * image;
    QPoint oldPosition;

public:
    explicit MyPaintImage(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);

signals:
    void sig_widgetResized(const QSize & newSize);
    void mousePositionChanged(const QVector2D & move_xy);
    void scrollPositionChanged(int scroll);
public slots:
    void slot_paintImage(QImage *new_image);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
private slots:
    void paintEvent (QPaintEvent *event);
private:

};

#endif // MYPAINTIMAGE_H
