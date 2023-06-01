#ifndef BoundingBox_h
#define BoundingBox_h

#include "Point.h"
#include <limits>

class BoundingBox {
private:
    int n;
    vector<double> minCoordinates;
    vector<double> maxCoordinates;
public:
    BoundingBox(int);
    BoundingBox(int, vector<double>, vector<double>);
    vector<double> getMinCoordinates() const;
    vector<double> getMaxCoordinates() const;
    double getN() const;
    double getArea() const;
    bool intersects(const BoundingBox& other) const;
    bool contains(const Point&) const;
    double getUpdatedArea(const BoundingBox&);
    void includeBox(const BoundingBox&);
    double calculateOverlap(const BoundingBox&) const;
    double calculateMargin() const;
    Point getCenter() const;
    bool compareBoundingBox(const BoundingBox&, int);
    void reset();
};

#endif