#include "myobjloader.h"

bool MyObjLoader::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
           return false;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(process_line(line) == -1) return false;
    }

    if(object->sizeTriangle() == 0) return false;
    return true;
}

int MyObjLoader::process_line(const QString &line)
{
    QStringList list = line.split(" ", QString::SkipEmptyParts);
    if(list.size() < 3) return false;
    if(list[0] == "f"){
        for(int i = 1; i < list.size(); i++){
            int good = readConnections(list[i]);
            if(!good) return good;
        }
        if(list.size() == 4){
            object->addTriangle(triangleId, triangleId+1, triangleId+2);
            triangleId+= 3;
        }
        else{
            QVector<int> polygons(list.size()-1);
            for(int i = 0; i < polygons.size(); i++){
                polygons[i] = triangleId++;
            }
            QVector<TriangleMeshObject::Trio> triangles = TriangleMeshObject::splitToTriangle(polygons);
            for(int i = 0; i < triangles.size(); i++){
                object->addTriangle(triangles[i]);
            }
        }
    }
    else if(list[0] == "vt"){
        if(list.size() < 3) return false;
        QVector3D vec;
        if(readVector(vec, list[1], list[2]) == false) return false;
        object->addTextureVector(vec);
    }
    else{
        if(list.size() < 4) return false;
        QVector3D vec;
        if(readVector(vec, list[1], list[2], list[3]) == false) return false;
        if(list[0] == "v"){
            object->addModelVector(vec);
        }
        else if(list[0] == "vn"){
            object->addNormalVector(vec);
        }
        else return false;
    }
    return true;
}

int MyObjLoader::readConnections(QString s)
{
    int modelId, textureId, normalId;
    int val;
    if(s.size() == 1){
        haveTexture = haveNormal = false;
        if(readInt(val, s) == false) return -1;
        object->addPoint(val-1);
        return true;
    }
    QStringList pointList = s.split("/");
    if(pointList.size() < 1) return -1;
    if(readInt(val, pointList[0]) == false) return -1;
    modelId = val-1;
    if(haveTexture){
        if(pointList.size() >= 2 && pointList[1] != "" && readInt(val, pointList[1])){
            textureId = val-1;
        }
        else haveTexture = false;
    }
    if(haveNormal){
        if(pointList.size() >= 3 && pointList[2] != "" && readInt(val, pointList[2])){
            normalId = val-1;
        }
        else haveNormal = false;
    }
    if(modelId >= object->sizeModel() || haveTexture && textureId >= object->sizeTexture() || haveNormal && normalId >= object->sizeNormal()) return false;
    if(haveNormal && haveTexture) object->addPoint(modelId, textureId, normalId);
    else if(haveTexture) object->addPointTexture(modelId, textureId);
    else if(haveNormal) object->addPointNormal(modelId, normalId);
    return true;
}

bool MyObjLoader::readVector(QVector3D & vec, const QString &s1, const QString &s2, const QString &s3)
{
    bool okX, okY, okZ;
    float x = s1.toFloat(&okX);
    float y = s2.toFloat(&okY);
    float z = s3.toFloat(&okZ);
    if(okX && okY && okZ){
        vec = QVector3D(x, y, z);
        return true;
    }
    return false;
}

bool MyObjLoader::readVector(QVector3D &vec, const QString &s1, const QString &s2)
{
    bool okX, okY;
    float x = s1.toFloat(&okX);
    float y = s2.toFloat(&okY);
    if(okX && okY){
        vec = QVector3D(x, y, 0);
        return true;
    }
    return false;
}

bool MyObjLoader::readInt(int &value, const QString &s)
{
    bool ok;
    value = s.toInt(&ok);
    if(ok) return true;
    return false;
}
