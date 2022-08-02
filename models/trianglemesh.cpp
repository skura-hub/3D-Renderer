#include "trianglemesh.h"

TriangleMeshObject::TriangleMeshObject()
{
    resetTextureImage();
    resetBorders();
    modelHaveNormals = modelHaveTexture = true;
    typeForOperator = Type::model;
    triangleIdForOperator = 0;
}

QVector3D &TriangleMeshObject::rPoint(int triangle_id, int point_id, TriangleMeshObject::Type vectorType)
{
    int id = triangles[triangle_id][point_id];
    if(vectorType == Type::model) return models[points[id][0]];
    if(vectorType == Type::texture) return textures[points[id][1]];
    return normals[points[id][2]];
}

const QVector3D &TriangleMeshObject::rPoint(int triangle_id, int point_id, TriangleMeshObject::Type vectorType) const
{
    int id = triangles[triangle_id][point_id];
    if(vectorType == Type::model) return models[points[id][0]];
    if(vectorType == Type::texture) return textures[points[id][1]];
    return normals[points[id][2]];
}

TriangleMeshObject &TriangleMeshObject::operator+=(const TriangleMeshObject &tm)
{
    models += tm.models;
    textures += tm.textures;
    normals += tm.normals;
    points += tm.points;
    triangles += tm.triangles;

    modelHaveTexture = modelHaveTexture && tm.modelHaveTexture;
    modelHaveNormals = modelHaveNormals && tm.modelHaveNormals;

    updateBorders(minBorders);
    updateBorders(maxBorders);
    return *this;
}

QVector<TriangleMeshObject::Trio> TriangleMeshObject::splitToTriangle(QVector<int> polygon, int joint)
{
    QVector<TriangleMeshObject::Trio>triangle(polygon.size()-2);
    int id = 0;
    for(int i = 0; i < joint - 1; i++){
        triangle[id][0] = polygon[i];
        triangle[id][1] = polygon[i+1];
        triangle[id][2] = polygon[joint];
        id++;
    }
    for(int i = joint + 1; i < polygon.size()-1; i++){
        triangle[id][0] = polygon[joint];
        triangle[id][1] = polygon[i];
        triangle[id][2] = polygon[i+1];
        id++;
    }
    if(joint != 0 && joint != polygon.size()-1){
        triangle.last()[0] = polygon[joint];
        triangle.last()[1] = polygon.last();
        triangle.last()[2] = polygon.first();
    }
    return triangle;
}

void TriangleMeshObject::resetBorders()
{
    minBorders = QVector3D(FLT_MAX, FLT_MAX, FLT_MAX);
    maxBorders = QVector3D(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

void TriangleMeshObject::updateBorders(const QVector3D &point)
{
    for(int i = 0; i < 3; i++){
        if(point[i] < minBorders[i]) minBorders[i] = point[i];
        if(point[i] > maxBorders[i]) maxBorders[i] = point[i];
    }
}
