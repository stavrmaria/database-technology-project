#ifndef BoundingBox_h
#define BoundingBox_h

#include "Point.h"

class BoundingBox {
private:
    double minX;
    double minY;
    double maxX;
    double maxY;
public:
    BoundingBox();
    BoundingBox(double, double, double, double);
    double getMinX() const;
    double getMinY() const;
    double getMaxX() const;
    double getMaxY() const;
    double getArea() const;
    bool intersects(const BoundingBox& other) const;
    bool contains(const Point&) const;
    void update(const Point&);
};

#endif