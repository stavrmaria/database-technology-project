#include "Point.h"

// empty point constructor
Point::Point() {
    this->n = 0;
    this->id = 0;
    this->name = "";
    this->coordinates.clear();
}

// point constructor based on it's attributes
Point::Point(unsigned long long id, string name, vector<double> coordinates) {
    this->n = this->coordinates.size();
    this->id = id;
    this->name = name;
    this->coordinates = coordinates;
}

// point constructor based on it's attributes
Point::Point(vector<double> coordinates) {
    this->n = coordinates.size();
    this->id = 0;
    this->name = "";
    this->coordinates = coordinates;
}

// Copy constructor
Point::Point(const Point &point) {
    this->n = point.n;
    this->id = point.id;
    this->name = point.name;
    this->coordinates = point.coordinates;
}

// Destructor
Point::~Point() {
    this->coordinates.clear();
}

// return the number of dimensions
int Point::getN() const {
    return this->n;
}

// return the point ID
unsigned long long Point::getID() const {
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
void Point::setID(unsigned long long id) {
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

    representation += to_string(this->id) + ",";
    representation += this->name + ",";
    
    for (int i = 0; i < this->n; i++)
        representation += to_string(this->coordinates.at(i)) + ",";

    representation.erase(representation.length() - 1);
    return representation;
}

ostream &operator<<(ostream &stream,const Point &point){
    stream << point.toString();
    return stream;
}