#ifndef MYRENDERERZBUFF_H
#define MYRENDERERZBUFF_H

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QSize>
#include <QDebug>
#include <QtGlobal>
#include <QtMath>
#include <QImage>
#include <QPair>

#include "trianglemesh.h"
#include "camera.h"
#include "matrixtransformation.h"
#include "mypainter.h"



class myRendererZBuff : public QObject
{
    Q_OBJECT
private:
    class funCon : public MyPainter::ScanlineHandler{
    private:
        int width;
        int height;
        int length;
        QVector<float> * zTab;
        QVector<QPair<int, int>> * idTab;
        TriangleMeshObject * t;
        int objectId;
    public:
        funCon(int width_, int height_, QVector<float> * zTab_pointer, QVector<QPair<int, int>> * idTab_pointer, TriangleMeshObject * triangles, int objectId_) : ScanlineHandler()
            {width = width_; height = height_; zTab = zTab_pointer; idTab = idTab_pointer; t = triangles; objectId = objectId_;
             length = width * height;}
        void scanlineHandler(int x, int y);
        static bool betterZ(float best_Z, float temp_z)
            {return qFabs(temp_z) < best_Z;}
    };

    QVector<float> depthBuffer;
    QVector<QPair<int, int>> triangleIndexer;
    QVector <MatrixTransformation> * transformations; //pointer
    QSize * size; //pointer
    QImage * image; // new delete
    float aspect;
    const QVector<TriangleMeshObject>* objects;
    QVector<TriangleMeshObject> pixel;
public:
   explicit  myRendererZBuff(const QVector <TriangleMeshObject> * objects_pointer,QVector <MatrixTransformation> * transformations_pointer,QSize * size_pointer, QObject *parent);
    ~myRendererZBuff();

signals:
    void sig_imageRendered(QImage * newImage);
public slots:
    void slot_renderingSizeChanged();
    void slot_objectsChanged();
    void slot_cameraChanged();

private:
    void renderImage();
    void resizeToBlank();
    void normalizedToPixel(int i);
    void normalizedToPixel(QSize size, QVector3D & vec);
    QVector3D vectorTransformation(int objectId, const QVector3D & point3D);
    void normalizePoints(int i);
    void pseudoClipping(int i);
    static int index(int x, int y, int width)
        {return x + width * y;}
    bool inNormalizedRange(const QVector3D &vector);
    void setToPixelTexture(int objectId);

};



#endif // MYRENDERERZBUFF_H
