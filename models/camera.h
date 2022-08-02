#ifndef CAMERA_H
#define CAMERA_H

#include<QVector3D>
#include<QSize>
#include <QMatrix4x4>
#include <QtGlobal>
#include <QtMath>

enum class Projection{perspective, orthographic};

class CameraView{
private:
    QVector3D cameraPosition;
    QVector3D cameraTarget;
    QVector3D up;

public:
    CameraView(){}
    CameraView(const QVector3D & cameraPosition_, const QVector3D & cameraTarget_, const QVector3D & up_){
        cameraPosition = cameraPosition_;
        cameraTarget = cameraTarget_;
        up = up_;
    }
    void setCameraPosition(const QVector3D & cameraPosition_)
        {cameraPosition = cameraPosition_;}
    void addCameraPosition(const QVector3D & cameraPosition_)
        {cameraPosition += cameraPosition_;}
    void setCameraTarget(const QVector3D & cameraTarget_)
        {cameraTarget = cameraTarget_;}
    void setCameraUpwardsDirection(const QVector3D & upwardsDirection)
        {up = upwardsDirection;}

    QVector3D getCameraPosition() const
        {return cameraPosition;}
    QVector3D getCameraTarget() const
        {return cameraTarget;}
    QVector3D getCameraUpwardsDirection() const
        {return up;}

    CameraView rotatePosition(float degreeX, float degreeY) const;

};
class CameraProjection{
private:
    float far;
    float near;
    QSize size;
    Projection type;
public:
    CameraProjection(){}
    CameraProjection(QSize size_, float far_, float near_, Projection type_ = Projection::perspective)
        {far = far_; near = near_; size = size_; type = type_;}
    void setScreenDistances(float distancesNear, float distancesFar)
        {near = distancesNear; far = distancesFar;}
    void setScreenSize(float width, float hight)
        {size = QSize(width, hight);}
    void setScreenSize(const QSize & screenSize)
        {size = screenSize;}
    void setType(Projection type_)
        {type = type_;}

    QSize getScreenSize() const
        {return size;}
    float getFar() const
        {return far;}
    float getNear() const
        {return near;}
    Projection getType() const
        {return type;}
};

#endif // CAMERA_H
