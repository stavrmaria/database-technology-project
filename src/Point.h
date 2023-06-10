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
        unsigned long int id;
        string name;
        vector<double> coordinates;
        int n;
    public:
        Point();
        Point(const Point&);
        Point(int, string, vector<double>);
        Point(vector<double>);
        int getN() const;
        unsigned long int getID() const;
        string getName() const;
        double getDimension(int) const;
        double getDistance(Point&) const;
        vector<double> getCoordinates() const;
        void setID(unsigned long int);
        void setName(string);
        void setDimension(int, double);
        void addDimension(double);
        string toString() const;
        bool dominates(Point&);
        friend ostream &operator<<(ostream&,const Point&);
};

#endif