#include "Point.h"

// empty point constructor
Point::Point() {
    this->n = 0;
    this->id = "-";
    this->name = "-";
}

// point constructor based on it's attributes
Point::Point(string id, string name, vector<double> coordinates) {
    this->n = this->coordinates.size();
    this->id = "-";
    this->name = "-";
    this->coordinates = coordinates;
}

// return the number of dimensions
int Point::getN() const {
    return this->n;
}

// return the point ID
string Point::getID() const {
    return this->id;
}


// return the name of the point
string Point::getName() const {
    return this->name;
}

// get the value of the dimension at index
double Point::getDimension(int index) const {
    if (index < 0 || index >= this->n)
        return -1;
    return this->coordinates.at(index);
}

// calculate and return the distance between two points
double Point::getDistance(Point &point) const {
    double total = 0;
    for (int i = 0; i < this->n; i++)
        total += pow(this->coordinates[i] - point.coordinates[i], 2);

    return sqrt(total);
}

// get the coordinates of the point
vector<double> Point::getCoordinates() const {
    return this->coordinates;
}

// setter for the point's id
void Point::setID(string id) {
    this->id = id;
}

// setter for the point's name
void Point::setName(string name) {
    this->name = name;
}

// setter for the point's dimension at index
void Point::setDimension(int index, double value) {
    if (index < 0 || index >= this->n)
        return;
    this->coordinates.at(index) = value;
}

// add a dimension to the point
void Point::addDimension(double value) {
    this->coordinates.push_back(value);
    this->n = this->coordinates.size();
}

// return the string representation of the point
string Point::toString() const {
    string representation = "";

    if (this->coordinates.size() == 0)
        return nullptr;

    representation += "id:" + this->id + ",";
    representation += "name:" + this->name + ",";
    
    for (int i = 0; i < this->n; i++)
        representation += to_string(this->coordinates.at(i)) + ",";

    representation.erase(representation.length() - 1);
    return representation + "\n";
}

ostream &operator<<(ostream &stream,const Point &point){
    stream << point.toString();
    return stream;
}