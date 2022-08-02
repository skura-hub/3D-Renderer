#include "mypainter.h"

MyPainter::MyPainter(QImage *imageToDraw, QObject *parent) :QObject (parent)
{
    setImage(imageToDraw);
    color = QColor(Qt::black).rgb();
}


void MyPainter::setImage(QImage *imageToDraw)
{
    image = imageToDraw;
    cImage = imageToDraw;
}

void MyPainter::setImage(const QImage *imageToRead)
{
    cImage = imageToRead;
}

void MyPainter::setColor(QRgb newColor)
{
    color = newColor;
}

void MyPainter::setColorC(const QColor &newColor)
{
    setColor(newColor.rgb());
}

void MyPainter::validPoints(int &x, int &y) const
{
    if(!image->valid(x, y)){
        if(x < 0) x = 0;
        else if(x > image->width()) x = image->width();
        if(y < 0) y = 0;
        else if(y > image->height()) y = image->height();
    }
}


void MyPainter::setPixel(int x, int y, uint index_or_rgb)
{
    if(!image->valid(x, y)) return;
    QRgb * rowData = reinterpret_cast<QRgb*>(image->scanLine(y));
    rowData += x;
    *rowData = index_or_rgb;
    QColor c(index_or_rgb);
}

void MyPainter::setPixel(const QPoint &point, uint index_or_rgb)
{
    setPixel(point.x(), point.y(), index_or_rgb);
}

void MyPainter::setPixel(int x, int y, const QColor &color)
{
    setPixel(x, y, color.toRgb());
}

void MyPainter::setPixel(const QPoint &point, const QColor &color)
{
    setPixel(point.x(), point.y(), color.toRgb());
}


QRgb MyPainter::getPixel(int x, int y) const
{
    if(cImage->valid(x, y)) return 0;
    const QRgb * rowData = reinterpret_cast<const QRgb*>(cImage->scanLine(y));
    rowData += x;
    const QRgb color = *rowData;
    return color;
}

QRgb MyPainter::getPixel(const QPoint &point) const
{
    return getPixel(point.x(), point.y());
}

QColor MyPainter::getPixelColor(int x, int y) const
{
    QColor color (getPixel(x, y));
    return color;
}

QColor MyPainter::getPixelColor(const QPoint &point) const
{
    return getPixelColor(point.x(), point.y());
}

QVector<QRgb> MyPainter::getPixel() const
{
    QVector<QRgb> pixels;
    pixels.resize(cImage->height() * cImage->width());
    memmove(pixels.data(), cImage->bits(), cImage->height() * cImage->width() * sizeof(QRgb));
    return pixels;
}



void MyPainter::drawLine(const QPoint &p1, const QPoint &p2)
{
    drawLine(p1.x(), p1.y(), p2.x(), p2.y());
}

void MyPainter::drawLine(int x1, int y1, int x2, int y2)
{
    double a,b;
    //if(!image->valid(x1, y1) || !image->valid(x2, y2)) return;
    validPoints(x1, y1);
    validPoints(x2, y2);
    //if(x1 == x2 && y1 == y2) return;
    if(x1 == x2){
        if( x1 > x2)    std::swap( x2, x1);
        if( y1 > y2)    std::swap( y2, y1);

        int x = x1;
        for(int y=y1; y<=y2; y++)
        {
            setPixel(x, y, color);
        }
    }
    else{
        a = (double) (y2-y1)/(x2-x1);
        b = (double) y2 - a * x2;

        if( x1 > x2)    std::swap( x2, x1);
        if( y1 > y2)    std::swap( y2, y1);

        if( (x1-x2) < (y1-y2) ){
            for(int x=x1; x<=x2; x++)
            {
                int y = qFloor((a*x+b)+ 0.5);
                setPixel(x, y, color);

            }
        }
        else{
            for(int y=y1; y<=y2; y++)
            {
                int x = qFloor(((y-b)/a)+0.5);
                setPixel(x, y, color);
            }
       }
    }
}

void MyPainter::drawCircle(const QPoint &srodek, double r)
{
    double a = r/2 * sqrt(2);
    for(int x = 0; x <= a; x++){
        int y = qFloor(sqrt(r*r - x*x) + 0.5);
        QVector <QPoint> punkty(8);
        punkty[0] = QPoint(srodek.x() + x, srodek.y() + y);
        punkty[1] = QPoint(srodek.x() + x, srodek.y() - y);
        punkty[2] = QPoint(srodek.x() - x, srodek.y() + y);
        punkty[3] = QPoint(srodek.x() - x, srodek.y() - y);

        punkty[4] = QPoint(srodek.x() + y, srodek.y() + x );
        punkty[5] = QPoint(srodek.x() - y, srodek.y() + x);
        punkty[6] = QPoint(srodek.x() + y, srodek.y() - x);
        punkty[7] = QPoint(srodek.x() - y, srodek.y() - x);

        for(int i = 0; i < 8; i++){
            setPixel(punkty[i], color);
        }
    }
}

void MyPainter::drawElipse(const QPoint &srodek, double r1, double r2, double beta)
{
    if(qFuzzyCompare(r1, r2)) drawCircle(srodek, r1);
    int N = 20;
    beta = beta * M_PI / 180;
    QPoint pierwszy, temp;
    for(int i = 0; i < N; i++){
        double alfa = 2 * M_PI * i / N;
        double x = r1 * cos(alfa);
        double y = r2 * sin(alfa);
        QPoint punkt;
        double x2 = x*cos(beta) - y * sin(beta);
        double y2 = x*sin(beta) + y * cos(beta);
        punkt.setX(qFloor(srodek.x()+ x2+0.5));
        punkt.setY(qFloor(srodek.y() + y2+ 0.5));
        if(i!= 0) drawLine(temp, punkt);
        else pierwszy = punkt;
        temp = punkt;
    }
    drawLine(temp, pierwszy);
}

void MyPainter::drawSplineBezier(const QVector <QPoint> & points)
{
    int i = 0;
    QPoint punkt, temp;
    bool pierwszy = true;
    while(i + 3 < points.size()){
        double t;
        double N = 100;
        for(double j = 0; j <= N; j++){
            t = j / N;
            double punkt_x, punkt_y;
            punkt_x = qPow(1-t,3) * points[i].x() + 3 * qPow(1-t, 2)* t * points[i+1].x() + 3 * (1-t) * t*t *points[i+2].x() + t*t*t*points[i+3].x();
            punkt_y = punkt_y = qPow(1-t,3) * points[i].y() + 3 * qPow(1-t, 2)* t * points[i+1].y() + 3 * (1-t) * t*t *points[i+2].y() + t*t*t*points[i+3].y();
            punkt = QPoint(qFloor(punkt_x + 0.5), qFloor(punkt_y+0.5));
            if(pierwszy) pierwszy = false;
            else drawLine(temp, punkt);
            temp = punkt;
        }
        i+= 3;
    }
}

void MyPainter::drawSplineBasis(const QVector <QPoint> & points)
{
    double x,y,l,t;
        l=points.size();
        double N = 1000;
        for(int q =3; q< points.size(); q++){
            for(double j =0;j<N;j++){
                t = j/N;
                x=0; y=0;
                x += (1.0/6.0) * ( ((-t)*(-t)*(-t)) + (3.0*t*t) - (3.0*t) + 1.0) * points[0 + q-3].x();
                x += (1.0/6.0) * ( (3.0*(t)*(t)*(t)) - (6.0*t*t) + 4.0) * points[1 + q-3].x();
                x += (1.0/6.0) * ( (-3.0*(t)*(t)*(t)) + (3.0*t*t) + (3.0*t) + 1) * points[2 + q-3].x();
                x += (1.0/6.0) * ( (t*t*t) ) *points[3 + q-3].x();
                y += (1.0/6.0) * ( ((-t)*(-t)*(-t)) + (3.0*t*t) - (3.0*t) + 1) * points[0 + q-3].y();
                y += (1.0/6.0) * ( (3.0*(t)*(t)*(t)) - (6.0*t*t) + 4) * points[1 + q-3].y();
                y += (1.0/6.0) * ( (-3.0*(t)*(t)*(t)) + (3.0*t*t) + (3.0*t) + 1) * points[2 + q-3].y();
                y += (1.0/6.0) * ( (t*t*t) ) *(points[3 + q-3].y());
                setPixel(x,y, color);
            }
        }
}

void MyPainter::drawPolygon(const QVector<QPoint> &points)
{
    if(points.size() < 3) return;
    for(int i = 1; i < points.size(); i++){
        drawLine(points[i-1], points[i]);
    }
    drawLine(points.first(), points.last());
}

void MyPainter::drawPolygon(const QVector<QPoint> &points, const QVector<int> &connections, int indexBeggining)
{
    int & id = indexBeggining;
    if(points.size() < 3 || connections.size() < 3) return;
    for(int i = 1; i < connections.size(); i++){
        QColor c(0, i * 80, i * 80);
        color = c.rgb();
        drawLine(points[connections[i-1] - id], points[connections[i] - id]);
    }
    color = QColor(Qt::red).rgb();
    drawLine(points[connections.first() - id], points[connections.last() - id]);
}

void MyPainter::drawTriangle(const QPoint & p1, const QPoint & p2, const QPoint & p3)
{
    drawLine(p1, p2);
    drawLine(p2, p3);
    drawLine(p3, p1);
}

void MyPainter::drawRectangle(const QPoint & p1, const QPoint & p2, const QPoint & p3, const QPoint & p4)
{
    drawLine(p1, p2);
    drawLine(p2, p3);
    drawLine(p3, p4);
    drawLine(p4, p1);
}

void MyPainter::floodFill(const QPoint & point)
{
    QRgb zmieniany = getPixel(point);
    if(zmieniany != color) floodFill(point, zmieniany, color);
}

QRgb MyPainter::bilinearInterpolation(const QVector2D &point) const
{
    QPoint p4(point.x(), point.y());
    QPoint p1(p4.x(), p4.y() + 1);
    QPoint p3(p4.x()+1, p4.y());
    QPoint p2(p4.x()+1, p4.y()+1);
    float a = point.x() - p4.x();
    float b = point.y() - p4.y();
    QColor cp1, cp2, cp3, cp4;
    cp1 = getPixelColor(p1);
    cp2 = getPixelColor(p2);
    cp3 = getPixelColor(p3);
    cp4 = getPixelColor(p4);
    int cr,cg,cb;
    cr = bilinearInterpolation(cp1.red(), cp2.red(), cp3.red(), cp4.red(), a, b);
    cg = bilinearInterpolation(cp1.green(), cp2.green(), cp3.green(), cp4.green(), a, b);
    cb = bilinearInterpolation(cp1.blue(), cp2.blue(), cp3.blue(), cp4.blue(), a, b);
    QColor c(cr, cg, cb);
    return c.rgb();
}

QRgb MyPainter::bilinearInterpolation(const QVector2D &point, const QImage &image_from) const
{
    QPoint p4(point.x(), point.y());
    QPoint p1(p4.x(), p4.y() + 1);
    QPoint p3(p4.x()+1, p4.y());
    QPoint p2(p4.x()+1, p4.y()+1);
    float a = point.x() - p4.x();
    float b = point.y() - p4.y();
    QColor cp1, cp2, cp3, cp4;
    cp1 = getPixelColor(p1, image_from);
    cp2 = getPixelColor(p2, image_from);
    cp3 = getPixelColor(p3, image_from);
    cp4 = getPixelColor(p4, image_from);
    int cr,cg,cb;
    cr = bilinearInterpolation(cp1.red(), cp2.red(), cp3.red(), cp4.red(), a, b);
    cg = bilinearInterpolation(cp1.green(), cp2.green(), cp3.green(), cp4.green(), a, b);
    cb = bilinearInterpolation(cp1.blue(), cp2.blue(), cp3.blue(), cp4.blue(), a, b);
    QColor c(cr, cg, cb);
    return c.rgb();
}

void MyPainter::drawTriangle(const QImage &fromImage, const QVector<QVector2D> &fromTriangle, const QVector<QVector2D> &toTriangle)
{
    QVector<QVector2D>fillPoint;
    BaseHandler bh(this, &fillPoint);
    doScanLine(toTriangle, bh);
    for(int i = 0; i < fillPoint.size(); i++){
        QRgb color = getTriangleColor(fillPoint[i], fromImage,
                                      fromTriangle[0], fromTriangle[1], fromTriangle[2],
                                      toTriangle[0], toTriangle[1], toTriangle[2]);
        image->setPixel(fillPoint[i].x(), fillPoint[i].y(), color);
    }
}

QRgb MyPainter::getTriangleColor(const QVector2D & fillPoint, const QImage &fromImage,
                                 const QVector2D &fromTriangleA, const QVector2D & fromTriangleB, const QVector2D & fromTriangleC,
                                 const QVector2D &toTriangleA, const QVector2D &toTriangleB, const QVector2D &toTriangleC)
{
    QVector3D uvw = barricentric(fillPoint, toTriangleA, toTriangleB, toTriangleC);
    float u = uvw.x(), v = uvw.y(), w = uvw.z();
    //if(u >= 0 && u <= 1 && u >= 0 && v <= 1 && w >= 0 && w <= 1){
       float xa = fromTriangleA.x(), xb = fromTriangleB.x(), xc = fromTriangleC.x();
       float ya = fromTriangleA.y(), yb = fromTriangleB.y(), yc = fromTriangleC.y();
       float x = u*xa + v * xb + w *xc;
       float y = u*ya + v * yb + w *yc;
       QVector2D p(x, y);
       return bilinearInterpolation(p, fromImage);
    //}
    qDebug() << u << v << w;
    return 0;
}

QVector3D MyPainter::barricentric(const QVector2D &point, const QVector2D &a, const QVector2D &b, const QVector2D &c)
{
    float x = point.x();
    float xa = a.x();
    float xb = b.x();
    float xc = c.x();
    float y = point.y();
    float ya = a.y();
    float yb = b.y();
    float yc = c.y();
    float upV = (x-xa)*(yc-ya)-(y-ya)*(xc-xa);
    float upW = (xb-xa)*(y-ya)-(x-xa)*(yb-ya);
    float down = (xb -xa)*(yc-ya)-(xc-xa)*(yb-ya);
    QVector3D result;
    float v = upV / down;
    float w = upW / down;
    float u = 1 - v - w;
    result[0] = u;
    result[1] = v;
    result[2] = w;
    return result;
}

int MyPainter::bilinearInterpolation(int p1, int p2, int p3, int p4, float a, float b) const
{
    return b * ((1-a) * p1 + a * p2) + (1-b) * ((1-a) * p4 + a * p3);
}


void MyPainter::floodFill(const QPoint & point, QRgb color_from, QRgb color_to)
{
    QStack <QPoint> Q;
    Q.push(point);
    while(Q.empty() == false){
        QPoint p = Q.pop();
        if(image->valid(p)){
            QRgb c = getPixel(p);
            if(c == color_from){
                setPixel(p, color_to);
                Q.push(QPoint(p.x() - 1, p.y()));
                Q.push(QPoint(p.x() + 1, p.y()));
                Q.push(QPoint(p.x(), p.y() + 1));
                Q.push(QPoint(p.x(), p.y() - 1));
            }
        }
    }
}

QColor MyPainter::getPixelColor(const QPoint &p, const QImage &image)
{
    if(!image.valid(p.x(), p.y())) return QColor();
    const QRgb * rowData = reinterpret_cast<const QRgb*>(image.scanLine(p.y()));
    rowData += p.x();
    QRgb color = *rowData;
    return QColor(color);
}

void MyPainter::BaseHandler::scanlineHandler(int x, int y)
{
    if(type == Type::draw){
        p->setPixel(x, y, p->color);
    }
    else {
        result->push_back(QVector2D(x, y));
    }
}
