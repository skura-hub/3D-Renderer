#ifndef MYRENDERERBUGET_H
#define MYRENDERERBUGET_H

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QSize>
#include <QDebug>
#include <QtGlobal>
#include <QImage>

#include "mypainter.h"
#include "object3dbudget.h"
#include "camera3d.h"

class MyRendererBuget : public QObject
{
    Q_OBJECT
private:
    QVector <object3DBudget> * objects;
    QImage * image;
    Camera3D * camera;
    static const int firstColor = 4;
    static const int lastColor = 18;
public:
    explicit MyRendererBuget(QVector <object3DBudget> * objects_pointer, Camera3D * camera_pointer, QObject *parent);
    ~MyRendererBuget();

signals:
    void sig_imageRendered(QImage * newImage);
public slots:
    void slot_renderingSizeChanged();
    void slot_objectsChanged();
    void slot_cameraChanged();

private:
    void renderImage();
    void resizeToBlank();
    QPoint pointTransformation(const QVector3D & point3D, const QMatrix4x4 & transform);
    void renderObject(const object3DBudget & obj);


};

#endif // MYRENDERERBUGET_H
