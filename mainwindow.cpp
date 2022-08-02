#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ready = false;
    actualMaxPoint = 0;
    paintImage = new MyPaintImage(this);
    setCentralWidget(paintImage);
    size = paintImage->size();
    cameraView.setCameraTarget(QVector3D(0, 0, 0));
    cameraView.setCameraPosition(QVector3D(0, 0, -20));
    cameraView.setCameraUpwardsDirection(QVector3D(0, 1, 0));
    cameraProjection.setType(Projection::perspective);
    cameraProjection.setScreenSize(size);
    cameraProjection.setScreenDistances(0.1, 1000);
    MatrixTransformation::setProjectionMatrix(cameraProjection);
    MatrixTransformation::setViewMatrix(cameraView);
    renderer = new myRendererZBuff(&objects, &transformations, &size, this);
    toolBarObject = new QToolBar("Modele", this);
    toolBarTransform = new QToolBar("Transformacje", this);
    toolBarTransform->setMovable(true);
    toolBarTransform->setAllowedAreas(Qt::ToolBarArea::AllToolBarAreas);
    toolBarTransform->hide();

    actionToolBarTransformation = Q_NULLPTR;
    actionToolBarTransformationList = Q_NULLPTR;
    actionToolBarObject = Q_NULLPTR;

    addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolBarObject);
    listWidgetObjects = new MyListWidget(this);
    toolBarObject->addWidget(listWidgetObjects);
    toolBarObject->setMovable(true);
    toolBarObject->setAllowedAreas(Qt::ToolBarArea::AllToolBarAreas);


    connect(renderer,
            SIGNAL(sig_imageRendered(QImage *)),
            paintImage,
            SLOT(slot_paintImage(QImage *)) );
    connect(paintImage,
            SIGNAL(sig_widgetResized(const QSize &)),
            this,
            SLOT(slot_PaintImageSizeChanged(const QSize &)) );
    connect(paintImage,
            SIGNAL(mousePositionChanged(const QVector2D &)),
            this,
            SLOT(slot_cameraLocationChanged(const QVector2D &)) );
    connect(paintImage,
            SIGNAL(scrollPositionChanged(int)),
            this,
            SLOT(slot_cameraLocationChanged(int)) );

    connect(this,
            SIGNAL(sig_objectsChanged()),
            renderer,
            SLOT(slot_objectsChanged()) );
    connect(this,
            SIGNAL(sig_sizeChanged()),
            renderer,
            SLOT(slot_renderingSizeChanged()) );
    connect(this,
            SIGNAL(sig_cameraLocationChanged()),
            renderer,
            SLOT(slot_cameraChanged()) );

    connect(listWidgetObjects,
            SIGNAL(sig_itemAdded()),
            this,
            SLOT(slot_objectAdd()) );
    connect(listWidgetObjects,
            SIGNAL(sig_itemRemoved(int)),
            this,
            SLOT(slot_objectDeleted(int)) );
    connect(listWidgetObjects,
            SIGNAL(sig_currentRowChanged(int)),
            this,
            SLOT(slot_objectSelected(int)));

    this->showMaximized();
}


MainWindow::~MainWindow()
{

}

void MainWindow::slot_PaintImageSizeChanged(const QSize &newSize)
{
    size = newSize;
    cameraProjection.setScreenSize(newSize);
    MatrixTransformation::setProjectionMatrix(cameraProjection);
    emit(sig_sizeChanged());
}

void MainWindow::slot_cameraLocationChanged(int z)
{
    if(objects.size() == 0) return;
    cameraView.addCameraPosition(QVector3D(0, 0, z));
    CameraView cv = cameraView.rotatePosition(cameraRotation.x(), cameraRotation.y());
    MatrixTransformation::setViewMatrix(cv);
    emit(sig_cameraLocationChanged());
}

void MainWindow::slot_cameraLocationChanged(const QVector2D &delta)
{
    if(objects.size() == 0) return;
    cameraRotation += delta;
    if(cameraRotation.x() >= 360) cameraRotation[0] -= 360;
    if(cameraRotation.y() >= 360) cameraRotation[1] -= 360;
    CameraView cv = cameraView.rotatePosition(cameraRotation.x(), cameraRotation.y());
    MatrixTransformation::setViewMatrix(cv);
    emit(sig_cameraLocationChanged());
}

void MainWindow::slot_transformationAdd()
{
    transformations[selectedObject_id].push_back(Transformation());
    MyControllerTransformation * ct = new MyControllerTransformation(this);
    controllerTransformations[selectedObject_id].push_back(ct);

    connect(ct,
            SIGNAL(sig_transformationChanged(const Transformation &)),
            this,
            SLOT(slot_transformationChanged(const Transformation &)) );
}

void MainWindow::slot_transformationDelete(int id)
{
    MyControllerTransformation * ct = controllerTransformations[selectedObject_id][id];
    transformations[selectedObject_id].remove(id);
    controllerTransformations[selectedObject_id].remove(id);
    delete ct;
    emit(sig_objectsChanged());

}

void MainWindow::slot_transformationSelected(int id)
{
    /*for(int i = 0; i < transformations[selectedObject_id].size(); i++){
        qDebug() << transformations[selectedObject_id].r(i).getVector();
    }*/
    if(id >= controllerTransformations[selectedObject_id].size()) return;
    selectedTransformation_id = id;
    if(actionToolBarTransformation != Q_NULLPTR){
        toolBarTransform->removeAction(actionToolBarTransformation);
    }
    actionToolBarTransformation = toolBarTransform->addWidget(controllerTransformations[selectedObject_id][id]);
    actionToolBarTransformation->setVisible(true);
}

void MainWindow::slot_transformationReplaced(int id_old, int id_new)
{
    transformations[selectedObject_id].move(id_old, id_new);
    controllerTransformations[selectedObject_id].move(id_old, id_new);
    slot_transformationSelected(id_new);
    emit(sig_objectsChanged());
}

void MainWindow::slot_transformationChanged(const Transformation &transformation)
{
    transformations[selectedObject_id].replace(selectedTransformation_id, transformation);
    emit(sig_objectsChanged());
}

void MainWindow::slot_objectAdd()
{
    createControllerTransformation();
    QString parentPath = "C:\\Users\\reant\\Downloads\\modele";
    QString currentPath = QDir::currentPath();
    QString path = QFileDialog::getOpenFileName(this, "Otwórz model", parentPath, "3D Model (*.obj)");
    TriangleMeshObject * object = new TriangleMeshObject();
    MyObjLoader loader(object);
    if(loader.loadFile(path) == false){
        QMessageBox msgBox;
        msgBox.setText("Plik nie jest właściwy");
        msgBox.exec();
        listWidgetObjects->removeItem(listWidgetObjects->size()-1);
        return;
    }
    objects.push_back(*object);
    QVector3D newCenter = QVector3D(0, 0, 0);
    float object_width = object->getMaxBorders().x() - object->getMinBorders().x();
    float spaceing = object_width / 10;
    actualMaxPoint = actualMaxPoint + object_width + spaceing;
    if(objects.size() > 1){
        newCenter.setX(actualMaxPoint + object_width/2 + spaceing);
    }
    transformations.push_back(MatrixTransformation(newCenter));
    MyControllerObject * co = new MyControllerObject(this);
    controllerObject.push_back(co);
    connect(co,
            SIGNAL(sig_texture_added(const QImage &)),
            this,
            SLOT(slot_object_textureAdded(const QImage &)) );
    connect(co,
            SIGNAL(sig_texture_deleted()),
            this,
            SLOT(slot_object_textureDeleted()) );
    connect(co,
            SIGNAL(sig_vector_changed(const QVector3D &)),
            this,
            SLOT(slot_object_center_changed(const QVector3D &)) );
}

void MainWindow::slot_objectDeleted(int id)
{
    objects.remove(id);
    transformations.remove(id);
    MyControllerObject * co = controllerObject[id];
    controllerObject.remove(id);
    MyListWidget * lw = listWidgetTransformations[id];
    listWidgetTransformations.remove(id);
    for(int i = 0; i < controllerTransformations[id].size(); i++){
        MyControllerTransformation * ct = controllerTransformations[id][i];
        delete ct;
    }
    controllerTransformations.remove(id);
    delete lw;
    delete co;
    if(objects.size() == 0){
        toolBarTransform->hide();
    }
    emit(sig_objectsChanged());
}

void MainWindow::slot_objectSelected(int id)
{
    selectedObject_id = id;
    if(actionToolBarObject != Q_NULLPTR){
        toolBarObject->removeAction(actionToolBarObject);
    }
    controllerObject[id]->slot_setCenter(transformations[id].getCenter());
    actionToolBarObject = toolBarObject->addWidget(controllerObject[id]);
    actionToolBarObject->setVisible(true);
    QVector3D camera = transformations[selectedObject_id].getCenter() + (objects[id].getMaxBorders() + objects[id].getMinBorders())/2;
    cameraView.setCameraTarget(camera);
    QVector3D size = objects[selectedObject_id].getMaxBorders() - objects[selectedObject_id].getMinBorders();
    float m = qMax(size.x(), size.y());
    float extraZ = 2*m;
    camera.setZ(objects[selectedObject_id].getMinBorders().z() - extraZ);
    cameraView.setCameraPosition(camera);
    transformations[selectedObject_id].setViewMatrix(cameraView);
    cameraRotation = QVector2D(0, 0);
    emit(sig_cameraLocationChanged());

    changeToolBarTransform(id);
}

void MainWindow::slot_object_textureDeleted()
{
    objects[selectedObject_id].resetTextureImage();
    emit(sig_objectsChanged());
}

void MainWindow::slot_object_textureAdded(const QImage & newTexture)
{
    objects[selectedObject_id].setTextureImage(newTexture);
    emit(sig_objectsChanged());
}

void MainWindow::slot_object_center_changed(const QVector3D &newCenter)
{
    transformations[selectedObject_id].setCenter(newCenter);
    emit(sig_objectsChanged());
}

void MainWindow::createControllerTransformation()
{
    MyListWidget * lw = new MyListWidget(this, true);
    listWidgetTransformations.push_back(lw);
    controllerTransformations.push_back(QVector<MyControllerTransformation*>());
    connect(lw,
            SIGNAL(sig_itemAdded()),
            this,
            SLOT(slot_transformationAdd()) );
    connect(lw,
            SIGNAL(sig_itemRemoved(int)),
            this,
            SLOT(slot_transformationDelete(int)));
    connect(lw,
            SIGNAL(sig_itemMoved(int, int)),
            this,
            SLOT(slot_transformationReplaced(int, int)));
    connect(lw,
            SIGNAL(sig_currentRowChanged(int)),
            this,
            SLOT(slot_transformationSelected(int)));
}

void MainWindow::changeToolBarTransform(int id)
{
    if(actionToolBarTransformationList != Q_NULLPTR){
        toolBarTransform->removeAction(actionToolBarTransformationList);
    }
    if(actionToolBarTransformation != Q_NULLPTR){
        toolBarTransform->removeAction(actionToolBarTransformation);
    }
    toolBarTransform->setVisible(true);
    actionToolBarTransformationList = toolBarTransform->addWidget(listWidgetTransformations[id]);
    actionToolBarTransformationList->setVisible(true);
    addToolBar(Qt::ToolBarArea::RightToolBarArea, toolBarTransform);
    listWidgetTransformations[selectedObject_id]->setCurrentRow(0);
}
