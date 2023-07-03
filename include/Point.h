#ifndef Point_h
#define Point_h

#include <string>
#include <ostream>
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

class Point {
    private:
        unsigned long long id;
        string name;
        vector<double> coordinates;
        int n;

        string doubleToBinaryString(double) const;
    public:
        Point();
        Point(const Point&);
        Point(unsigned long long, string, vector<double>);
        Point(vector<double>);
        ~Point();
        int getN() const;
        unsigned long long getID() const;
        string getName() const;
        double getDimension(int) const;
        double getDistance(Point&) const;
        vector<double> getCoordinates() const;
        void setID(unsigned long long);
        void setName(string);
        void setDimension(int, double);
        void addDimension(double);
        string toString() const;
        friend ostream &operator<<(ostream&,const Point&);
        friend istream &operator>>(istream&, Point&);
        unsigned long long zOrderValue() const;

    void resetDimensions();
};

#endif