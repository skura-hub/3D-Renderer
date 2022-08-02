#ifndef MYOBJLOADER_H
#define MYOBJLOADER_H

#include<QFile>
#include<QTextStream>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QVector2D>

#include "trianglemesh.h"

class MyObjLoader
{
private:
    TriangleMeshObject * object;
    QString fileName;
    bool haveNormal;
    bool haveTexture;
    int triangleId;
public:
    MyObjLoader(TriangleMeshObject * object_pointer)
        {object = object_pointer; haveNormal = haveTexture = true; triangleId = 0;}
    bool loadFile(const QString &path);
private:
    int process_line(const QString & line);
    int readConnections(QString s);
    bool readVector(QVector3D & vec, const QString & s1, const QString & s2, const QString & s3);
    bool readVector(QVector3D & vec, const QString & s1, const QString & s2);
    bool readInt(int & value, const QString & s);
};

#endif // MYOBJLOADER_H
