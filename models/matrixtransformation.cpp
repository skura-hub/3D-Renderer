#include "matrixtransformation.h"

void MatrixTransformation::setProjectionMatrix(const CameraProjection &projectionInfo)
{
    float aspect = (float)projectionInfo.getScreenSize().width() / projectionInfo.getScreenSize().height();
    if(projectionInfo.getType() == Projection::perspective)
        projectionMatrix = projection_perspective(aspect, -aspect, -1, 1, 1, 100);
    else if(projectionInfo.getType() == Projection::orthographic)
        projectionMatrix = projection_orthographic(0, projectionInfo.getScreenSize().width(), 0, projectionInfo.getScreenSize().height(), 0.1, 1000);
    update();
}

void MatrixTransformation::setViewMatrix(const CameraView &viewInfo)
{
    cameraMatrix= projection_camera(viewInfo.getCameraPosition(), viewInfo.getCameraTarget(), viewInfo.getCameraUpwardsDirection());
    update();
}

void MatrixTransformation::swap(int id1, int id2)
{
    Transformation mt = transInfo[id1];
    transInfo[id1] = transInfo[id2];
    transInfo[id2] = mt;
}

QMatrix4x4 MatrixTransformation::transform(const Transformation & transformation) const
{
    switch (transformation.getType()) {
    case Transformation::Type::translation:
        return translationMatrix(transformation.getVector());
    case Transformation::Type::scale:
        return scaling(transformation.getVector());
    case Transformation::Type::shear:
        return shearing(transformation.getVector(), transformation.getAxis());
    case Transformation::Type::rotation:
        return rotateRelative(transformation.getAngle(), transformation.getAxis(), objectCenter);
    case Transformation::Type::rotation_relative:
        return rotateRelative(transformation.getAngle(), transformation.getAxis(), transformation.getVectorSpecial());
    default:
        return QMatrix4x4();
    }
}

const QMatrix4x4 MatrixTransformation::getResult() const
{
    QMatrix4x4 result = matrixAll;
    result *= centerTranslationMatrix;
    for(int i = 0; i < size(); i++){
        result *= transform(transInfo[i]);
    }
    return result;
}

QMatrix4x4 MatrixTransformation::translationMatrix(float x, float y, float z)
{
    QMatrix4x4 result;
    result(0, 3) = x;
    result(1, 3) = y;
    result(2, 3) = z;
    return result;
}

QMatrix4x4 MatrixTransformation::scaling(float x, float y, float z)
{
    QMatrix4x4 result;
    result(0, 0) = x;
    result(1, 1) = y;
    result(2, 2) = z;
    return result;
}

QMatrix4x4 MatrixTransformation::shearing(float x, float y, float z, Transformation::Axis axis)
{
    QMatrix4x4 result;
    if(axis == Transformation::Axis::OZ){
        result(0,2) = x;
        result(1,2) = y;
    }
    else if(axis == Transformation::Axis::OY){
        result(0,1) = x;
        result(2, 1) = z;
    }
    else{
        result(1,0) = y;
        result(2,0) = z;
    }
    return result;
}

QMatrix4x4 MatrixTransformation::rotateRad(float rad, Transformation::Axis axis)
{
    QMatrix4x4 result;
    float cos = qCos(rad);
    float sin = qSin(rad);
    if(axis == Transformation::Axis::OZ){
        result(0,0) = cos;
        result(0,1) = -sin;
        result(1,0) = sin;
        result(1,1) = cos;
    }
    else if(axis == Transformation::Axis::OY){
        result(0,0) = cos;
        result(0,2) = sin;
        result(2,0) = -sin;
        result(2,2) = cos;
    }
    else{
        result(1,1) = cos;
        result(1,2) = -sin;
        result(2,1) = sin;
        result(2,2) = cos;
    }
    return result;
}

QMatrix4x4 MatrixTransformation::projection_camera(const QVector3D &cameraPosition, const QVector3D &camera_target, const QVector3D &up)
{
    QVector3D forward_ = cameraPosition - camera_target;
    forward_.normalize();
    QVector3D side_ = QVector3D::crossProduct(up, forward_);
    side_.normalize();
    QVector3D up_ = QVector3D::crossProduct(forward_, side_);
    QMatrix4x4 result;
    result(0, 0) = side_.x();
    result(0, 1) = side_.y();
    result(0, 2) = side_.z();
    result(0,3) = -QVector3D::dotProduct(side_, cameraPosition);
    result(1, 0) = up_.x();
    result(1, 1) = up_.y();
    result(1, 2) = up_.z();
    result(1,3) = -QVector3D::dotProduct(up_, cameraPosition);
    result(2, 0) = forward_.x();
    result(2, 1) = forward_.y();
    result(2, 2) = forward_.z();
    result(2, 3) = -QVector3D::dotProduct(forward_, cameraPosition);
    //result(3,0) = result(3,1) = result(3,2) = 1;
    result(3, 3) = 1;
    return result;
}

QMatrix4x4 MatrixTransformation::projection_perspective(float left, float right, float bottom, float top, float nearVal, float farVal)
//QMatrix4x4 MatrixTransformation::projection_perspective(float fov, float AR, float nearVal, float farVal)
{
    QMatrix4x4 result;
    result(0,0) = (2 * nearVal) / (right - left);
    result(1,1) = (2 * nearVal) / (top - bottom);
    result(0,2) = (right + left) / (right - left);
    result(1,2) = (top + bottom) / (top - bottom);
    result(2,2) = -(farVal + nearVal) / (farVal - nearVal);
    result(3,2) = (-1);
    result(2,3) = -(2 * farVal * nearVal) / (farVal - nearVal);
    result(3,3) = 0;
    return result;
    /*
    QMatrix4x4 result;
    float tanHalfFOVInverse   =  1/ qTan( (fov/2) * (M_PI / 180) );
    result(0,0) = tanHalfFOVInverse; // near/right

    result(1,1) = AR * tanHalfFOVInverse; //near / top (top = right /AR)

    result(2,2) =  (nearVal) / (farVal - nearVal);
    result(2,3) =  (farVal * nearVal) / (farVal - nearVal);

    result(3,2) = -1;

    return result;
    */
}

QMatrix4x4 MatrixTransformation::projection_orthographic(float left, float right, float bottom, float top, float nearVal, float farVal)
{

    QMatrix4x4 result;
    result(0,0) = (2) / (right - left);
    result(1,1) = (2) / (top - bottom);
    result(2,2) = -2 / (farVal - nearVal);
    result(3, 0) = -(left + right) / (right-left);
    result(3,1) = -(top + bottom) / (top - bottom);
    result(3,2) = -(farVal + nearVal)/(farVal - nearVal);
    //result(3,3) = 1;
    //result(0,1) = result(0,2) = result(0,3) = result(1, 0) = result(1,2) = result(1,3) = result(2,0)= result(2,1) = result(2,3) = 1;
    return result;
}

QMatrix4x4 MatrixTransformation::projection_depth(float nearVal, float farVal)
{
    QMatrix4x4 result;
    float c1 = 2*farVal*nearVal / (nearVal - farVal);
    float c2 = (farVal + nearVal) / (farVal - nearVal);
    result(2,2) = -c2;
    result(2,3) = c1;
    result(3,2) = -1;
    result(3,3) = 1;
    return result;
}
