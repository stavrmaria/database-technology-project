#include "BoundingBox.h"

BoundingBox::BoundingBox() {
    this->minX = 0;
    this->minX = 0;
    this->minX = 0;
    this->minX = 0;
}

BoundingBox::BoundingBox(double minX, double minY, double maxX, double maxY) {
    this->minX = minX;
    this->minX = minY;
    this->minX = maxX;
    this->minX = maxY;
}

double BoundingBox::getMinX() const {
    return this->minX;
}

double BoundingBox::getMinY() const {
    return this->minY;
}

double BoundingBox::getMaxX() const {
    return this->maxX;
}

double BoundingBox::getMaxY() const {
    return this->maxY;
}

double BoundingBox::getArea() const {
    double width = this->maxX - this->minX;
    double height = this->maxY - this->minY;
    return width * height;
}

// check if a point is inside the bounding box
bool BoundingBox::contains(const Point &point) const {
    if (point.getLatitude() < this->minX || point.getLatitude() > this->maxX)
        return false;
    
    if (point.getLongitude() < this->minY || point.getLongitude() > this->maxY)
        return false;
    
    return true;
}

// update the bounding's box min and max coordinates based on a new point
void BoundingBox::update(const Point &point) {
    if (point.getLatitude() <= this->minX && point.getLongitude() <= this->minY) {
        this->minX = point.getLatitude();
        this->minY = point.getLongitude();
    }
    
    if (point.getLatitude() >= this->maxX && point.getLongitude() >= this->maxY) {
        this->maxX = point.getLatitude();
        this->maxY = point.getLongitude();
    }
}