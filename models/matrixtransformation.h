#ifndef MATRIXTRANSFORMATION_H
#define MATRIXTRANSFORMATION_H
#include<QMatrix4x4>
#include<QVector3D>
#include<QVector>
#include<QtGlobal>
#include<QtMath>

#include "camera.h"

class Transformation{
public:
    enum class Type{ translation, scale, shear, rotation, rotation_relative, none};
    enum class Axis{ OX, OY, OZ};
private:
    Type type;
    QVector3D vector1;
    QVector3D vector2;
    float angle;
    Axis axis;
public:
    Transformation(){type = Type::none;}
    void setTranslation(const QVector3D & vector){
        type = Type::translation;
        vector1 = vector;
    }
    void setScale(const QVector3D & vector){
        type = Type::scale;
        vector1 = vector;
    }
    void setShear(const QVector3D & vector_, Axis axis_){
        type = Type::shear;
        vector1 = vector_;
        axis = axis_;
    }
    void setRotation(float angle_, Axis axis_){
        type = Type::rotation;
        angle = angle_;
        axis = axis_;
    }
    void setRotation(float angle_, Axis axis_, const QVector3D & rotation_center){
        type = Type::rotation_relative;
        angle = angle_;
        axis = axis_;
        vector2 = rotation_center;
    }
    Type getType() const
        {return type;}
    QVector3D getVector() const
        {return vector1;}
    QVector3D getVectorSpecial() const
        {return vector2;}
    float getAngle() const
        {return angle;}
    Axis getAxis() const
        {return axis;}
};

class MatrixTransformation
{
private:
    QVector3D objectCenter;
    QVector<Transformation>transInfo;
    QMatrix4x4 centerTranslationMatrix;
    QMatrix4x4 resultMatrix;
    static QMatrix4x4 projectionMatrix;
    static QMatrix4x4 cameraMatrix;
    static QMatrix4x4 matrixAll;

public:   
    MatrixTransformation(const QVector3D & center){ setCenter(center);}
    static void setProjectionMatrix(const CameraProjection & projectionInfo);
    static void setViewMatrix(const CameraView & viewInfo);

    QVector3D getCenter() const
        {return objectCenter;}
    void setCenter(const QVector3D & center)
        {objectCenter = center; centerTranslationMatrix = translationMatrix(center);}
    int size() const
        {return transInfo.size();}
    const Transformation & cr(int id) const
        {return transInfo[id];}
    Transformation & r(int id)
        {return transInfo[id];}
    Transformation & last()
        {return transInfo.last();}
    Transformation & first()
        {return transInfo.first();}
    const QMatrix4x4 getResult() const;

    void updateForMapping()
        {resultMatrix = getResult();}
    QVector4D map(const QVector4D & vector) const
        {return resultMatrix * vector;}
    QVector4D map(const QVector3D & vector) const
        {return map(QVector4D(vector, 1));}
    void push_back(Transformation transformation)
        {transInfo.push_back(transformation);}
    void insert(int id, Transformation transformation)
        {transInfo.insert(id, transformation);}
    void replace(int id, Transformation transformation)
        {transInfo.replace(id, transformation);}
    void remove(int id)
        {transInfo.remove(id);}
    void swap(int id1, int id2);
    void move(int from, int to)
        {transInfo.move(from, to);}
    void removeAll()
        {transInfo.clear();}

    static QMatrix4x4 translationMatrix(float x, float y, float z);
    static QMatrix4x4 translationMatrix(const QVector3D & vector)
        {return translationMatrix(vector.x(), vector.y(), vector.z());}
    static QMatrix4x4 scaling(float x, float y, float z);
    static QMatrix4x4 scaling(const QVector3D &vector)
        {return scaling(vector.x(), vector.y(), vector.z());}
    static QMatrix4x4 shearing(float x, float y, float z, Transformation::Axis axis);
    static QMatrix4x4 shearing(const QVector3D & vector, Transformation::Axis axis)
        {return shearing(vector.x(), vector.y(), vector.z(), axis);}
    static QMatrix4x4 rotateRad(float rad, Transformation::Axis axis);
    static QMatrix4x4 rotate(float angle, Transformation::Axis axis)
        {angle = angle * M_PI / 180.0;
        return rotateRad(angle, axis);}
    static QMatrix4x4 rotateRelative(float angle, Transformation::Axis axis, const QVector3D & relativePosition)
        {return translationMatrix(-relativePosition) * rotate(angle, axis) * translationMatrix(relativePosition);}
    static QMatrix4x4 projection_camera(const QVector3D & cameraPosition, const QVector3D & camera_target, const QVector3D & up = QVector3D({0, 1, 0}));
    static QMatrix4x4 projection_perspective(float left, float right, float bottom, float top, float nearVal, float farVal);
    //static QMatrix4x4 projection_perspective(float fov, float AR, float nearVal, float farVal);
    static QMatrix4x4 projection_orthographic(float left, float right, float bottom, float top, float nearVal, float farVal);
    static QMatrix4x4 projection_depth(float nearVal, float farVal);
    static QVector4D map(const QVector4D & vector, const QMatrix4x4 & matrix)
        {return matrix * vector;}
    static QVector4D map(const QVector3D & vector, const QMatrix4x4 & matrix)
        {return map(QVector4D(vector, 1), matrix);}
private:
    QMatrix4x4 transform(const Transformation & transformation) const;
    static void update()
        {matrixAll =  projectionMatrix *  cameraMatrix;}
};

#endif // MATRIXTRANSFORMATION_H
