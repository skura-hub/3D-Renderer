#include "camera.h"
#include "matrixtransformation.h"

CameraView CameraView::rotatePosition(float degreeX, float degreeY) const
{
    QMatrix4x4 matrix = MatrixTransformation::rotateRelative(degreeX, Transformation::Axis::OY, cameraTarget);
    matrix *= MatrixTransformation::rotateRelative(degreeY, Transformation::Axis::OX, cameraTarget);
    CameraView result = *this;
    result.cameraPosition = MatrixTransformation::map(cameraPosition, matrix).toVector3D();
    return result;
}
