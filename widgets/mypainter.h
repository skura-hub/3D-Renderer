#ifndef MYPAINTER_H
#define MYPAINTER_H

#include <QObject>
#include <QImage>

#include<QtMath>
#include<QStack>
#include<QVector>
#include<QDebug>
#include<QVector2D>
#include<QVector3D>

class MyPainter : public QObject
{
    Q_OBJECT
public:
    class ScanlineHandler{
    public:
        virtual void scanlineHandler(int x, int y) = 0;
        virtual ~ScanlineHandler(){}
    };
private:
    class BaseHandler : public ScanlineHandler{
    public:
        enum class Type{draw, get};

        BaseHandler(MyPainter * painter) : ScanlineHandler(){
            p = painter;
            type = Type::draw;
        }
        BaseHandler(MyPainter * painter, QVector<QVector2D> * result_) : ScanlineHandler(){
            p = painter;
            result = result_;
            type = Type::get;
        }
        void scanlineHandler(int x, int y);


    private:
        MyPainter * p;
        Type type;
        QVector<QVector2D> * result;
};

    QImage * image;
    const QImage * cImage;
    QRgb color;
public:
    MyPainter(QImage *imageToDraw, QObject *parent = nullptr);
    MyPainter(const QImage * imageToRead, QObject * parent = nullptr) : QObject(parent)
        {cImage = imageToRead; image = Q_NULLPTR;}
    void setImage(QImage * imageToDraw);
    void setImage(const QImage * imageToRead);
    void setColor(QRgb newColor);
    void setColorC(const QColor & newColor);
    void validPoints(int & x, int & y) const;
    void setPixel(int x, int y, uint index_or_rgb);
    void setPixel(const QPoint & point, uint index_or_rgb);
    void setPixel(int x, int y, const QColor &color);
    void setPixel(const QPoint & point, const QColor & color);
    QRgb getPixel(int x, int y) const;
    QRgb getPixel(const QPoint & point) const;
    QColor getPixelColor(int x, int y) const;
    QColor getPixelColor(const QPoint & point) const;
    QVector<QRgb> getPixel() const;

    void drawLine(const QPoint &p1, const QPoint &p2);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawCircle(const QPoint &center, double r);
    void drawElipse(const QPoint &center, double rx, double ry, double rotation);
    void drawSplineBezier(const QVector <QPoint> & points);
    void drawSplineBasis(const QVector <QPoint> & points);

    void drawPolygon(const QVector <QPoint> & points);
    void drawPolygon(const QVector<QPoint> &points, const QVector <int> & connections, int indexBeggining = 0);
    void drawTriangle(const QPoint & p1, const QPoint & p2, const QPoint & p3);
    void drawRectangle(const QPoint & p1, const QPoint & p2, const QPoint & p3, const QPoint & p4);

    void floodFill(const QPoint & point);

    QRgb bilinearInterpolation(const QVector2D & point) const;
    QRgb bilinearInterpolation(const QVector2D & point, const QImage & image_from) const;
    void drawTriangle(const QImage & fromImage, const QVector<QVector2D> & fromTriangle, const QVector<QVector2D> & toTriangle);
    QRgb getTriangleColor(const QVector2D & fillPoint, const QImage &fromImage,
                          const QVector2D &fromTriangleA, const QVector2D & fromTriangleB, const QVector2D & fromTriangleC,
                          const QVector2D &toTriangleA, const QVector2D &toTriangleB, const QVector2D &toTriangleC);
    static QVector3D barricentric(const QVector2D & point, const QVector2D & a, const QVector2D & b, const QVector2D & c);

    template<class PointType>
    void drawScanLine(const PointType & points)
        {BaseHandler bh(this);
        doScanLine(points, bh);}
    template<class PointType>
    QVector<QVector2D> getScanline(const PointType & points)
        {QVector<QVector2D> result;
        BaseHandler bh(this, &result);
        doScanLine(points, bh);
        return result;
    }

    template<class PointType>
    void static doScanLine(const PointType & points, ScanlineHandler & actionObject){
        if(points.size() >= 3){
            int firstYmin = (int)points[0].y();
            int lastY = (int)points[0].y();
            for(int i = 1; i < points.size(); i++){
                if((int)points[i].y() < firstYmin) firstYmin = (int)points[i].y();
                else if((int)points[i].y() > lastY) lastY = (int)points[i].y();
            }

            std::vector <ET> * tab_ET = new std::vector<ET> [lastY-firstYmin];
            std::vector <ET> tab_AET;

            int y_min;
            int x_max, x_min;
            for(int i = 0; i < points.size(); i++){
                int id1 = i;
                int id2 = i-1;
                if(i == 0) id2 = points.size()-1;

                if((int)points[id1].y() != (int)points[id2].y()){
                    ET et;
                    if((int)points[id1].y() > (int)points[id2].y()){
                        et.y_max = (int)points[id1].y();
                        x_max = (int)points[id1].x();
                        y_min = (int)points[id2].y();
                        x_min = (int)points[id2].x();
                    }
                    else{
                        et.y_max = (int)points[id2].y();
                        x_max = (int)points[id2].x();
                        y_min = (int)points[id1].y();
                        x_min = (int)points[id1].x();
                    }
                    et.m = (x_max - x_min) /  (et.y_max - y_min);
                    et.x = x_min;

                    int id = y_min - firstYmin;
                    auto it = std::lower_bound(tab_ET[id].begin(), tab_ET[id].end(), et);
                    tab_ET[id].insert(it, et);
                }
            }
            for(int y = firstYmin; y < lastY || !tab_AET.empty(); y++){

                int id = y - firstYmin;
                if(y < lastY && !tab_ET[id].empty()){
                    for(auto it = tab_ET[id].begin(); it != tab_ET[id].end(); ++it){
                        auto itET = std::lower_bound(tab_AET.begin(), tab_AET.end(),*it);
                        tab_AET.insert(itET, *it);
                    }
                }
                for (auto it = tab_AET.begin(); it != tab_AET.end();) {
                    if (it->y_max <= y) {
                        it = tab_AET.erase(it);

                    }
                    else if(it != tab_AET.end()) ++it;
                }

                if(tab_AET.size() > 1){
                    bool nieparzyste= true;
                    for(auto it = tab_AET.begin(); it != tab_AET.end()-1; it++){
                        std::vector<ET>::iterator next;
                        next = it+1;
                        if(nieparzyste){
                            for(int x = it->x; x <= next->x; x++){
                                actionObject.scanlineHandler(x, y);
                            }
                        }
                    nieparzyste = !nieparzyste;
                    }
                }
                for (auto it = tab_AET.begin(); it != tab_AET.end(); it++) {
                        it->x += it->m;
                }
                std::sort(tab_AET.begin(), tab_AET.end());
            }
            delete [] tab_ET;

        }
    }

signals:

public slots:

private:
    int bilinearInterpolation(int p1, int p2, int p3, int p4, float a, float b) const;
    void floodFill(const QPoint & point, QRgb color_from, QRgb color_to);
    static QColor getPixelColor(const QPoint & p, const QImage & image);

    struct ET{
        double y_max;
        double x;
        double m;
        bool operator < (const ET & et){
            return x < et.x;
        }
    };
};

#endif // MYPAINTER_H
