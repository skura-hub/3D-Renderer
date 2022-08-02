#include "mypaintimage.h"

MyPaintImage::MyPaintImage(QWidget *parent) : QWidget(parent)
{
    image = Q_NULLPTR;
}

void MyPaintImage::resizeEvent(QResizeEvent *event)
{
    event->size();
    emit(sig_widgetResized(event->size()));
}

void MyPaintImage::paintEvent(QPaintEvent *event)
{
    QPainter painter (this);
    if( image != Q_NULLPTR ){
        painter.drawImage(0, 0, *image);
    }
    else{
        QImage imageToDraw (height(), width(), QImage::Format_RGB32);
        imageToDraw.fill(Qt::white);
        painter.drawImage(0, 0, imageToDraw);
    }
}


void MyPaintImage::slot_paintImage(QImage *new_image)
{
    image = new_image;
    update();
}

void MyPaintImage::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {

    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        emit(scrollPositionChanged(numSteps.x()+numSteps.y()));
    }
    event->accept();
}

void MyPaintImage::mouseMoveEvent(QMouseEvent *event)
{
    QVector2D delta(event->pos() - oldPosition);
    delta[0] = delta[0] / this->width() * 360/2;
    delta[1] = delta[1] / this->height() * 360/2;
    emit(mousePositionChanged(delta));
}

void MyPaintImage::mousePressEvent(QMouseEvent *event)
{
    oldPosition = event->pos();
}
