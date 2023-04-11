#ifndef Point_h
#define Point_h

#include <string>
#include <ostream>
#include <vector>
#include <cmath>
#include <unordered_map>

using namespace std;

class Point {
    private:
        string id;
        double lat;
        double lon;
        unordered_map<string, string> attributes;
        int dimensions;
    
    public:
        Point();
        Point(int);
        string getID() const;
        double getLatitude() const;
        double getLongitude() const;
        double getDistance(Point&) const;
        void setID(string);
        void setLatitude(double);
        void setLongitude(double);
        void addAttribute(string, string);
        void removeAttribute(string);
        string getAttribute(string) const;
        string toString() const;
        friend ostream &operator<<(ostream&,const Point&);
};

#endif