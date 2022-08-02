#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QDir>

#include "camera.h"
#include "mypaintimage.h"
#include "myrendererzbuff.h"
#include "trianglemesh.h"
#include "matrixtransformation.h"
#include "mycontrollertransformation.h"
#include "mylistwidget.h"
#include "mycontrollerobject.h"
#include "myobjloader.h"

#include <QtGlobal>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    MyPaintImage * paintImage;
    myRendererZBuff * renderer;

    QSize size;
    CameraView cameraView;
    CameraProjection cameraProjection;
    QVector2D cameraRotation;

    int selectedObject_id;
    int selectedTransformation_id;
    //int selectedTransformation_id;
    QToolBar * toolBarTransform;
    QAction* actionToolBarTransformation;
    QVector <MatrixTransformation> transformations;
    QVector<QVector<MyControllerTransformation*>> controllerTransformations;
    QVector<MyListWidget *> listWidgetTransformations;
    QAction * actionToolBarTransformationList;

    MyListWidget * listWidgetObjects;
    QAction * actionToolBarObject;
    QToolBar * toolBarObject;
    QVector<MyControllerObject*> controllerObject;
    QVector <TriangleMeshObject> objects;


    float actualMaxPoint;
    float ready;


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slot_PaintImageSizeChanged(const QSize &newSize);
    void slot_cameraLocationChanged(const QVector2D & delta);
    void slot_cameraLocationChanged(int z);

    void slot_transformationAdd();
    void slot_transformationDelete(int id);
    void slot_transformationSelected(int id);
    void slot_transformationReplaced(int id_old, int id_new);
    void slot_transformationChanged(const Transformation & transformation);

    void slot_objectAdd();
    void slot_objectDeleted(int id);
    void slot_objectSelected(int id);
    void slot_object_textureAdded(const QImage & texture);
    void slot_object_textureDeleted();
    void slot_object_center_changed(const QVector3D & newCenter);
signals:
    void sig_objectsChanged();
    void sig_sizeChanged();
    void sig_cameraLocationChanged();

private:
    void createControllerTransformation();
    void changeToolBarTransform(int id);
};

#endif // MAINWINDOW_H
