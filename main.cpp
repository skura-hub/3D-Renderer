#include "mainwindow.h"
#include "matrixtransformation.h"
#include <QApplication>

QMatrix4x4 MatrixTransformation::cameraMatrix = QMatrix4x4();
QMatrix4x4 MatrixTransformation::projectionMatrix = QMatrix4x4();
QMatrix4x4 MatrixTransformation::matrixAll = QMatrix4x4();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
