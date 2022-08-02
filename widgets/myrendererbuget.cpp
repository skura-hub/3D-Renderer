#include "myrendererbuget.h"

MyRendererBuget::MyRendererBuget(QVector <object3DBudget> * objects_pointer, Camera3D * camera_pointer, QObject *parent) : QObject(parent)
{
    objects = objects_pointer;
    camera = camera_pointer;
    image = new QImage(camera_pointer->getScreenSize(), QImage::Format_RGB32);
}

MyRendererBuget::~MyRendererBuget()
{
    delete image;
}

void MyRendererBuget::slot_renderingSizeChanged()
{
    resizeToBlank();
    renderImage();
}

void MyRendererBuget::slot_objectsChanged()
{
    renderImage();
}

void MyRendererBuget::slot_cameraChanged()
{
    renderImage();
}

void MyRendererBuget::renderImage()
{
    qDebug() << "renderowanie" << camera->getCameraPosition();
    resizeToBlank();
    for(int i = 0; i < objects->size(); i++){
        object3DBudget obj = objects->operator[](i);
        renderObject(obj);
    }

    emit(sig_imageRendered(image));
}

void MyRendererBuget::resizeToBlank()
{
    delete image;
    const QSize & size = camera->getScreenSize();
    image = new QImage(size.width(), size.height(), QImage::Format_RGB32);
    image->fill(Qt::white);
}


QPoint MyRendererBuget::pointTransformation(const QVector3D & point3D, const QMatrix4x4 & transform)
{
    QVector4D transformedPoint(point3D);
    transformedPoint.setW(1);
    transformedPoint = camera->matrix() * transformedPoint;
    //transformedPoint/= transformedPoint.w();
    //transformedPoint /= transformedPoint.z();
    float aspect = (float) camera->getScreenSize().width() / camera->getScreenSize().height();
    //if(transformedPoint.x() < - aspect || transformedPoint.x() > aspect || transformedPoint.y() < -aspect || transformedPoint.y() > aspect) return QPoint(-1,-1);
    qDebug() << transformedPoint;
    if(transformedPoint.x() < -aspect) transformedPoint[0] = -aspect;
    else if(transformedPoint.x() > aspect) transformedPoint[0] = aspect;
    if(transformedPoint.y() < -aspect) transformedPoint[1] = -aspect;
    else if(transformedPoint.y() > aspect) transformedPoint[1] = aspect;
    int width = camera->getScreenSize().width();
    int height = camera->getScreenSize().height();
    int x =qMin((double)width - 1, ((transformedPoint.x() + 1) * 0.5 * width));
    int y = qMin((double)height - 1, ((1 - (transformedPoint.y() + 1) * 0.5) * height));
    return QPoint(x, y);
}

void MyRendererBuget::renderObject(const object3DBudget & obj)
{
    QVector<QPoint> points2D;
    QMatrix4x4 transform = camera->matrix();
    for(int j = 0; j < obj.sizePoints(); j++){
        points2D.push_back(pointTransformation(obj.getPoint(j), transform));
    }
    MyPainter painter(image, this);
    for(int i = 0; i < obj.sizeFaces(); i++){
        QVector<QPoint> points;
        for(int j = 0; j <obj.sizeFace(i); j++){
            const QPoint & p = points2D[obj.getFace(i)[j]];
            points.push_back(p);
            painter.drawCircle(points.last(), 5);
        }
        int colorId = i%(lastColor-firstColor) + firstColor;
        painter.setColorC((QColor((Qt::GlobalColor)colorId)));
        painter.drawScanLine(points);
        painter.setColorC(Qt::black);
        painter.drawPolygon(points);
    }
}
