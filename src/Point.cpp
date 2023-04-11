#include "Point.h"

Point::Point() {
    this->dimensions = 3;
    this->id = "-";
    this->lat = 0;
    this->lon = 0;
}

Point::Point(int dimensions) {
    this->dimensions = dimensions;
    this->id = "-";
    this->lat = 0;
    this->lon = 0;
}

string Point::getID() const {
    return this->id;
}
        
double Point::getLatitude() const {
    return this->lat;
}

double Point::getLongitude() const {
    return this->lon;
}

double Point::getDistance(Point &point) const {
    return sqrt(pow(this->lat - point.lat, 2) + pow(this->lon - point.lon, 2));
}

void Point::setID(string id) {
    this->id = id;
}

void Point::setLatitude(double lat) {
    this->lat = lat;
}

void Point::setLongitude(double lon) {
    this->lon = lon;
}

void Point::addAttribute(string key, string value) {
    attributes[key] = value;
}

void Point::removeAttribute(string key) {
    this->attributes.erase(key);
}

string Point::getAttribute(string key) const {
    return attributes.at(key);
}

string Point::toString() const {
    string representation = "";

    if (this->attributes.size() == 0)
        return "empty\n";

    representation += "id:" + this->id + ",";
    representation += "lat:" + to_string(this->lat) + ",";
    representation += "lon:" + to_string(this->lon) + ",";
    
    for (const auto& attribute : this->attributes) {
        if (attribute.first == "id")
            continue;
        
        representation += attribute.first + ":" + attribute.second + ",";
    }

    representation.erase(representation.length() - 1);
    return representation + "\n";
}

ostream &operator<<(ostream &stream,const Point &point){
    stream << point.toString();
    return stream;
}