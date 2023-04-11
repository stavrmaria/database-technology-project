#ifndef RStarTree_h
#define RStarTree_h

#include "Point.h"
#include "Node.h"

#include <iostream>
#include <limits>

using namespace std;

class RStarTree {
    private:
        Node *root;
    public:
        RStarTree();
        ~RStarTree();
        void insert(Point);
        Point *search(Point);
};

#endif