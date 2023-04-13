#ifndef Point_h
#define Point_h

#include <string>
#include <ostream>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

class Point {
    private:
        string id;
        string name;
        vector<double> coordinates;
        int n;
    public:
        Point();
        Point(string, string, vector<double>);
        int getN() const;
        string getID() const;
        string getName() const;
        double getDimension(int) const;
        double getDistance(Point&) const;
        vector<double> getCoordinates() const;
        void setID(string);
        void setName(string);
        void setDimension(int, double);
        void addDimension(double);
        string toString() const;
        friend ostream &operator<<(ostream&,const Point&);
};

#endif