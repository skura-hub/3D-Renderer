#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <QVector3D>
#include <QVector>
#include <QDebug>
#include <QImage>

class TriangleMeshObject
{
public:
    struct Trio{
        int i1;
        int i2;
        int i3;
        Trio(){}
        Trio(int i1_, int i2_, int i3_)
            {i1 = i1_; i2 = i2_; i3 = i3_;}
        Trio(int i1_, int i2_)
            {i1 = i1_; i2 = i2_;}
        Trio(int i1_)
            {i1 = i1_;}
        int & operator[](int id){
            if(id == 0) return i1;
            else if(id == 1) return i2;
            return i3;
        }
        const int & operator[](int id) const{
            if(id == 0) return i1;
            else if(id == 1) return i2;
            return i3;
        }

    };
    enum class Type{model, texture, normal};
private:
    QVector<QVector3D> models;
    QVector<QVector3D> textures;
    QVector<QVector3D> normals;
    QVector<Trio> points;
    QVector<Trio> triangles;
    QImage imgTexture;
    bool textureEnabled;
    bool modelHaveTexture;
    bool modelHaveNormals;
    Type typeForOperator;
    int triangleIdForOperator;

    QVector3D minBorders;
    QVector3D maxBorders;
public:
    TriangleMeshObject();
    int sizeTriangle() const
        {return triangles.size();}
    int sizePoints() const
        {return points.size();}
    int sizeModel() const
        {return models.size();}
    int sizeTexture() const
        {return textures.size();}
    int sizeNormal() const
        {return normals.size();}

    void addModelVector(const QVector3D & model)
        {models.push_back(model);
         updateBorders(model);
        }
    void addTextureVector(const QVector3D & texture)
        {textures.push_back(texture);}
    void addNormalVector(const QVector3D & normal)
        {normals.push_back(normal);}
    void addPoint(int model_id, int texture_id, int normal_id)
        {points.push_back(Trio(model_id, texture_id, normal_id));}
    void addPoint(int model_id)
        {points.push_back(Trio(model_id));
        modelHaveNormals = modelHaveTexture = false;}
    void addPointTexture(int model_id, int texture_id)
        {points.push_back(Trio(model_id, texture_id));
        modelHaveNormals = false;}
    void addPointNormal(int model_id, int normal_id)
        {Trio t(model_id); t.i3 = normal_id;
        points.push_back(t);
        modelHaveTexture = false;}
    void addTriangle(int triangle1_id, int triangle2_id, int triangle3_id)
        {triangles.push_back(Trio(triangle1_id, triangle2_id, triangle3_id));}
    void addTriangle(Trio triangle)
        {triangles.push_back(triangle);}
    void removeTriangle(int id)
        {triangles.remove(id);}
    QVector3D & rModel(int id)
        {return models[id];}
    QVector3D & rTexture(int id)
        {return textures[id];}
    QVector3D & rNormal(int id)
        {return normals[id];}
    const QVector3D & rModel(int id) const
        {return models[id];}
    const QVector3D & rTexture(int id) const
        {return textures[id];}
    const QVector3D & rNormal(int id) const
        {return normals[id];}
    QVector3D & rPoint(int triangle_id, int point_id, Type vectorType = Type::model);
    const QVector3D & rPoint(int triangle_id, int point_id, Type vectorType = Type::model) const;

    void setTypeForOperator(Type vectorType)
        {typeForOperator = vectorType;}
    Type getTypeForOperator() const
        {return typeForOperator;}
    void setTriangleIdForOperator(int id)
        {triangleIdForOperator = id;}
    int getTriangleIdForOperator() const
        {return triangleIdForOperator;}
    int size() const
        {return 3;}
    QVector3D &operator[](int id)
        {return rPoint(triangleIdForOperator, id, typeForOperator);}
    const QVector3D & operator[](int id) const
        {return rPoint(triangleIdForOperator, id, typeForOperator);}

    void setTextureImage(const QImage & image)
        {imgTexture = image; textureEnabled = true;}
    const QImage & rTextureImage() const
        {return imgTexture;}
    void resetTextureImage()
        {textureEnabled = false;}
    bool haveTextureImage() const
        {return textureEnabled;}
    bool haveModelTexture() const
        {return modelHaveTexture;}
    bool haveModelNormals() const
        {return modelHaveNormals;}
    QVector3D getMinBorders() const
        {return minBorders;}
    QVector3D getMaxBorders() const
        {return maxBorders;}
    QVector3D optimalCenter() const
        {return (maxBorders + minBorders) / 2;}

    TriangleMeshObject & operator+=(const TriangleMeshObject & tm);
    static TriangleMeshObject createCube(int size);
    static QVector<TriangleMeshObject::Trio>splitToTriangle(QVector<int>polygon, int joint = 0);

private:
    void resetBorders();
    void updateBorders(const QVector3D & point);



};

#endif // TRIANGLEMESH_H
