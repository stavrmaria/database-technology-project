#include "BoundingBox.h"

// constructor based on the number of dimensions
BoundingBox::BoundingBox(int n) {
    this->n = n;
    for (int i = 0; i < n; i++) {
        this->minCoordinates.push_back(numeric_limits<double>::infinity());
        this->maxCoordinates.push_back(0);
    }
}

// constructor based on the number of dimensions and it's coordinates
BoundingBox::BoundingBox(int n, vector<double> minCoordinates, vector<double> maxCoordinates) {
    this->n = n;
    this->minCoordinates = minCoordinates;
    this->maxCoordinates = maxCoordinates;
}

// minimum coordinates getter
vector<double> BoundingBox::getMinCoordinates() const {
    return this->minCoordinates;
}

// maximum coordinates getter
vector<double> BoundingBox::getMaxCoordinates() const {
    return this->maxCoordinates;
}

// n dimensions getter
double BoundingBox::getN() const {
    return this->n;
}

// get the area of the bounding box
double BoundingBox::getArea() const {
    double area = 1;
    for (int i = 0; i < this->n; i++) {
        double side = this->maxCoordinates.at(i) - this->minCoordinates.at(i);
        area *= side;
    }

    return area;
}

// check if a point is inside the bounding box
bool BoundingBox::contains(const Point &point) const {
    for (int i = 0; i < this->n; i++) {
        if (point.getDimension(i) < this->minCoordinates.at(i))
            return false;    
        if (point.getDimension(i) > this->maxCoordinates.at(i))
            return false;
    }

    return true;
}

// Update the bounding's box min and max coordinates based on a new point
// and return the new area
double BoundingBox::getUpdatedArea(const BoundingBox &boundingBox) {
    vector<double> minCoordinates = this->minCoordinates;
    vector<double> maxCoordinates = this->maxCoordinates;
    double area = 1;

    for (int i = 0; i < this->n; i++) {
        if (boundingBox.minCoordinates.at(i) < minCoordinates.at(i))
            minCoordinates.at(i) = boundingBox.minCoordinates.at(i);
        
        if (boundingBox.maxCoordinates.at(i) > maxCoordinates.at(i))
            maxCoordinates.at(i) = boundingBox.maxCoordinates.at(i);
    }

    for (int i = 0; i < this->n; i++) {
        double side = maxCoordinates.at(i) - minCoordinates.at(i);
        area *= side;
    }

    return area;
}

void BoundingBox::includeBox(const BoundingBox &box) {
    for (int i = 0; i < this->n; i++) {
        if (box.minCoordinates.at(i) < this->minCoordinates.at(i))
            this->minCoordinates.at(i) = box.minCoordinates.at(i);
        
        if (box.maxCoordinates.at(i) > this->maxCoordinates.at(i))
            this->maxCoordinates.at(i) = box.maxCoordinates.at(i);
    }
}

// This function checks whether or not this bounding box intersects with this one
bool BoundingBox::intersects(const BoundingBox& other) const {
    for (int i = 0; i < this->n; i++) {
        if (this->minCoordinates.at(i) > other.maxCoordinates.at(i))
            return false;
        if (this->maxCoordinates.at(i) < other.minCoordinates.at(i))
            return false;
    }

    return true;
}