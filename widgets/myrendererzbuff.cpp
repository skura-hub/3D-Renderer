#include "myrendererzbuff.h"

myRendererZBuff::myRendererZBuff(const QVector <TriangleMeshObject> * objects_pointer, QVector <MatrixTransformation> * transformations_pointer, QSize * size_pointer, QObject *parent) : QObject(parent)
{
    objects = objects_pointer;
    transformations = transformations_pointer;
    size = size_pointer;
    aspect = (float)size->width()/size->height();
    image = new QImage(*size, QImage::Format_RGB32);
}

myRendererZBuff::~myRendererZBuff()
{
    delete image;
}

void myRendererZBuff::slot_renderingSizeChanged()
{
    aspect = (float)size->width()/size->height();
    renderImage();
}

void myRendererZBuff::slot_objectsChanged()
{
    renderImage();
}

void myRendererZBuff::slot_cameraChanged()
{
    renderImage();
}

void myRendererZBuff::renderImage()
{
    if(objects->size() < 1) return;
    pixel = *objects;
    resizeToBlank();
    const int length = size->width() * size->height();
    depthBuffer = QVector<float>(length, INT_MAX);
    triangleIndexer= QVector<QPair<int, int>>(length, QPair<int, int>(-1, -1));
    for(int i = 0; i < pixel.size(); i++){
        if((*objects)[i].haveModelTexture() && (*objects)[i].haveTextureImage())
            setToPixelTexture(i);
        normalizePoints(i);
        pseudoClipping(i);
        normalizedToPixel(i);
        funCon fun(size->width(), size->height(), &depthBuffer, &triangleIndexer, &pixel[i], i);
        pixel[i].setTypeForOperator(TriangleMeshObject::Type::model);
        for(int j = 0; j < pixel[i].sizeTriangle(); j++){
            pixel[i].setTriangleIdForOperator(j);
            MyPainter::doScanLine(pixel[i], fun);
        }
    }

    MyPainter P(image, this);
    for(int i = 0; i < length; i++){
        const int triangleId = triangleIndexer[i].first;
        const int objectId = triangleIndexer[i].second;
        if(triangleId != -1){
            int x = i % size->width();
            int y = i / size->width();
            if((*objects)[objectId].haveModelTexture() && (*objects)[objectId].haveTextureImage()){
                const QImage & img = (*objects)[objectId].rTextureImage();
                QRgb color = P.getTriangleColor(QVector2D(x, y), img,
                                   pixel[objectId].rPoint(triangleId, 0, TriangleMeshObject::Type::texture).toVector2D(),
                                   pixel[objectId].rPoint(triangleId, 1, TriangleMeshObject::Type::texture).toVector2D(),
                                   pixel[objectId].rPoint(triangleId, 2, TriangleMeshObject::Type::texture).toVector2D(),
                                   pixel[objectId].rPoint(triangleId, 0, TriangleMeshObject::Type::model).toVector2D(),
                                   pixel[objectId].rPoint(triangleId, 1, TriangleMeshObject::Type::model).toVector2D(),
                                   pixel[objectId].rPoint(triangleId, 2, TriangleMeshObject::Type::model).toVector2D() );
                P.setPixel(x, y, color);
            }
            else{
                QColor c = Qt::gray;
                P.setPixel(x, y, c.rgb());
            }

        }
    }
    emit(sig_imageRendered(image));
}

void myRendererZBuff::resizeToBlank()
{
    delete image;
    image = new QImage(*size, QImage::Format_RGB32);
    image->fill(Qt::white);
}


void myRendererZBuff::normalizedToPixel(int i)
{
    for(int j = 0; j < pixel[i].sizeModel(); j++){
        pixel[i].rModel(j)[0] = (int) qMin((float)size->width() - 1, ((pixel[i].rModel(j)[0] + 1) * (float)0.5 * size->width()));
        pixel[i].rModel(j)[1] =(int) qMin((float)size->height() - 1, ((1 - (pixel[i].rModel(j)[1] + 1) * (float)0.5) * size->height()));
    }
}
void myRendererZBuff::normalizedToPixel(QSize size, QVector3D & vec){
    vec[0] = qMin((float)size.width() - 1, (float)(((vec[0])) * size.width()));
    vec[1] = qMin((float)size.height() - 1, (float)((1-(vec[1])) * size.height()));
}

QVector3D myRendererZBuff::vectorTransformation(int objectId, const QVector3D &point3D)
{
    QVector4D vec =  transformations->operator[](objectId).map(point3D);
    if(!qFuzzyIsNull(vec.w())) vec /= vec.w();
    return vec.toVector3D();
}

void myRendererZBuff::normalizePoints(int i)
{
    transformations->operator[](i).updateForMapping();
    for(int j = 0; j < pixel[i].sizeModel(); j++){
        pixel[i].rModel(j) = vectorTransformation(i, pixel[i].rModel(j));
    }
}
void myRendererZBuff::pseudoClipping(int i)
{
    int size = pixel[i].sizeTriangle();
    for(int j = 0; j < size; j++){
        bool allIn1, allIn2, allIn3;
        allIn1 = inNormalizedRange(pixel[i].rPoint(j, 0));
        allIn2 = inNormalizedRange(pixel[i].rPoint(j, 1));
        allIn3 = inNormalizedRange(pixel[i].rPoint(j, 2));
        if(allIn1 || allIn2 || allIn3){

        }
        else{
            pixel[i].removeTriangle(j);
            j--;
            size--;
        }
    }
}

bool myRendererZBuff::inNormalizedRange(const QVector3D &vector)
{
    if(vector.x() < -aspect || vector.x() > aspect || vector.y() < -aspect || vector.y() > aspect) return false;
    return true;
}

void myRendererZBuff::setToPixelTexture(int objectId)
{
    const QImage & textureImg = (*objects)[objectId].rTextureImage();
    QSize size = textureImg.size();
    for(int i = 0; i < pixel[objectId].sizeTexture(); i++){
        normalizedToPixel(size, pixel[objectId].rTexture(i));
        qDebug() << pixel[objectId].rTexture(i);
    }
}

void myRendererZBuff::funCon::scanlineHandler(int x, int y)
{
    if(x < 0 || y < 0) return;
    const int id = (x+y*width);
    if(id >= length || id < 0) return;
    QVector3D bc_screen  = MyPainter::barricentric(QVector2D(x, y), (*t)[0].toVector2D(), (*t)[1].toVector2D(), (*t)[2].toVector2D());
    float z = 0;
    for (int i=0; i<3; i++) z += (*t)[i].z() * bc_screen[i];

    if (betterZ((*zTab)[id], z)) {
        (*zTab)[id] = z;
        (*idTab)[id].first = t->getTriangleIdForOperator();
        (*idTab)[id].second = objectId;
    }
}
